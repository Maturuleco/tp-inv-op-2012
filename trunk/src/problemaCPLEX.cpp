#include "problemaCPLEX.hpp"

/** PUBLIC ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
problemaCPLEX::problemaCPLEX()
:status(0), lp(NULL), tipo(BRANCH_AND_BOUND)
{
	env = CPXopenCPLEX(&status);

	if (env == NULL) {
		cerr << "Error creando el entorno." << endl;
	}

	relajacionEnRaiz = 0.0;
	tiempoDeCallback = 0.0;
	tiempoDeOptimizacion = 0.0;
	tiempoPreOptimizacion = 0.0;
	hayRelajacionEnRaiz = false;
} /* constructor para iniciar el problema CPLEX */


// destructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
problemaCPLEX::~problemaCPLEX()
{
	if ( lp != NULL ) {
		status = CPXfreeprob (env, &lp);
		if ( status ) {
			fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
		}
	}

	if ( env != NULL ) {
		status = CPXcloseCPLEX (&env);

      if ( status ) {
			char errmsg[CPXMESSAGEBUFSIZE];
			fprintf (stderr, "Could not close CPLEX environment.\n");
			CPXgeterrorstring (env, status, errmsg);
			fprintf (stderr, "%s", errmsg);
		}
	}
} /* destructor para liberar correctamente los recursos */


// optimizacion ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void problemaCPLEX::resolverMIP()
{
	double time_beg = 0.0;
	double time_end = 0.0;

	status = CPXgettime(env,&time_beg);
	status = CPXmipopt (env, lp);
	status = CPXgettime(env,&time_end);

	tiempoDeOptimizacion = time_end - time_beg;

	if ( status ) {
		fprintf (stderr, "Failed to optimize MIP.\n");
	}
} /* le avisamos a CPLEX que busque el optimo */


void problemaCPLEX::mostrarSolucion()
{
	double tolerancia = 1e-10;

//// muestro que algoritmo se corrio
	if (tipoDeOptimizacion() == CPX_MIN)
		printf("Minimizacion.\n");
	else if (tipoDeOptimizacion() == CPX_MAX)
		printf("Maximizacion.\n");
	else
		printf("Error al obtener sentido de optimizacion (min, max).\n");

	switch(tipo)
	{
		case BRANCH_AND_BOUND:
			printf("Branch & Bound.\n");
			break;
		case BRANCH_AND_CUT:
			printf("Branch & Cut.\n");
			break;
		case CUT_AND_BRANCH:
			printf("Cut & Branch.\n");
			break;
	}

//// muestro estado de la solucion
	int solstat = CPXgetstat (env, lp);
	char buffer[511];
	CPXCHARptr statstring = CPXgetstatstring(env,solstat,buffer);
	if (statstring == NULL)
		printf("Error al pedir SOLUTION STATUS.\n");
	else
		printf ("Solution status: %s.\n",buffer);

//// muestro funcional
	double objVal = optimo();
	printf ("MIP Objective value:\t\t\t %.10g\n", objVal );
	printf("LP Objetctive value:\t\t\t %.10g\n", relajacionEnRaiz);

//// muestro gap vs nodo raiz
	double gapVsRaiz = fabs( (100.0*(objVal - relajacionEnRaiz))/relajacionEnRaiz );
	printf("Gap vs Nodo Raiz:\t\t\t %.10g\n", gapVsRaiz);

//// muestro gap alcanzado
	double gapAlcanzado = 0.0;
	status = CPXgetmiprelgap(env, lp, &gapAlcanzado);
	printf("Gap final alcanzado:\t\t\t %.10g\n", gapAlcanzado);

//// muestro informacion del arbol de branching
	printf("Tiempo en PreOptimizacion:\t\t %.10g [segs]\n", tiempoPreOptimizacion);
	printf("Tiempo en Optimizar:\t\t\t %.10g [segs]\n",tiempoDeOptimizacion);
	printf("Tiempo en Buscar Cortes:\t\t %.10g [segs]\n", tiempoDeCallback);
	printf("Numero de variables:\t\t\t %d\n", numeroVariables());
	printf("Numero de restricciones:\t\t %d\n", numeroRestricciones());
	printf("Numero de nodos:\t\t\t %d\n", CPXgetnodecnt(env,lp));
	printf("Nodo de valor optimo:\t\t\t %d\n", CPXgetnodeint(env,lp));
	printf("Nodos que quedaron sin explorar:\t %d\n", CPXgetnodeleftcnt(env,lp));

//// muestro informacion de cortes agregados
	printf("Cortes Cover Dynamic Agregados>\t\t %d\n", mochilas.cuantosDinamicos());
	printf("Cortes Cover Greedy Agregados>\t\t %d\n", mochilas.cuantosGreedy());
	printf("Cortes Clique Agregados>\t\t %d\n", grafoDeConflictos.cuantosCortes());
	printf("Cantidad de ejes en Grafo Conflicto>\t %d\n",grafoDeConflictos.cuantosEjes());

//// muestro valores de las variables
//	int cur_numcols = numeroVariables();
//	double variables[cur_numcols];
//	for (int i = 0; i < cur_numcols; i++){
//		variables[i] = 0.0;
//	}
//	double * x = (double*)&variables;
//	status = CPXgetx (env, lp, x, 0, cur_numcols-1);
//	if ( status ) {
//		fprintf (stderr, "Failed to obtain solution.\n");
//		return;
//	}
//	for (int j = 0; j < cur_numcols; j++) {
//		if ( fabs (variables[j]) > tolerancia )
//			printf ( "Column %d:  Value = %.4f\n", j, variables[j]);
} /* mostrar por stdout los resultados obtenidos */


// const ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool problemaCPLEX::hayError() const
{
	return (status != 0);
} /* deteccion de errores en rutinas de CPLEX */


void problemaCPLEX::mostrarError() const
{
	char errmsg[CPXMESSAGEBUFSIZE];
	CPXCCHARptr translation = CPXgeterrorstring (env, status, errmsg);
	if (translation != NULL){
		fprintf (stderr, "%s", errmsg);
	} else {
		fprintf (stderr, "CPLEX ERROR %d.\n", status);
	}
} /* mostrar el error asociado a la rutina CPLEX */


enum ALGORITMO problemaCPLEX::elTipo() const
{
	return tipo;
} /* para saber que tipo de algoritmo se usa */


int problemaCPLEX::numeroRestricciones() const
{
	return CPXgetnumrows(env,lp);
} /* numero de restricciones originales del MIP */


int problemaCPLEX::numeroVariables() const
{
	return CPXgetnumcols(env,lp);
} /* numero de variables originales del MIP */


bool problemaCPLEX::optimoDeRelajacionAgregado() const
{
	return hayRelajacionEnRaiz;
} /* para saber si el valor del optimo en el nodo raiz se asigno en la instancia */


void problemaCPLEX::sumarTiempoDeCallback(double valor)
{
	tiempoDeCallback += valor;
} /* agregar tiempo transcurrido en el callback */


int problemaCPLEX::tipoDeOptimizacion() const
{
	return CPXgetobjsen(env,lp);
} /* devuelve CPX_MIN si es problema de minimizacion o sino CPX_MAX */


// obtener el funcional en su valor optimo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double problemaCPLEX::optimo()
{
    double objval;
    status = CPXgetobjval(env, lp, &objval);

    if (status) {
        cerr << "Problema obteniendo mejor solución entera." << endl;
    }
    return objval;
} /* optimo de la solucion entera */


// configuracion del problema ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void problemaCPLEX::deshabilitarParametros()
{
	int params[] = {CPX_PARAM_PREIND,CPX_PARAM_PRELINEAR,CPX_PARAM_EACHCUTLIM,
					CPX_PARAM_CUTPASS,CPX_PARAM_FRACCUTS,CPX_PARAM_HEURFREQ,
					CPX_PARAM_RINSHEUR,CPX_PARAM_REDUCE,CPX_PARAM_IMPLBD,
					CPX_PARAM_MCFCUTS,CPX_PARAM_ZEROHALFCUTS,CPX_PARAM_MIRCUTS,
					CPX_PARAM_GUBCOVERS,CPX_PARAM_FLOWPATHS,CPX_PARAM_FLOWCOVERS,
					CPX_PARAM_DISJCUTS,CPX_PARAM_COVERS,CPX_PARAM_CLIQUES,
					CPX_PARAM_THREADS,CPX_PARAM_MIPSEARCH,CPX_PARAM_MIPCBREDLP};

	int values[] =  {0,0,0,0,-1,-1,-1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,1,1,CPX_OFF};

	for (int e = 0; e < 21; e++)
	{
		status = CPXsetintparam(env, params[e], values[e]);
		if (status != 0) break;
	}

	if (status) {
		fprintf(stderr,"Falla al deshabilitar parametros.\n");
	}
} /* configuro parametros CPLEX */


void problemaCPLEX::leerLP(const char* ruta)
{
	lp = CPXcreateprob (env, &status, "mip_para_tp");

	if (status) return;

	status = CPXreadcopyprob(env, lp, ruta, NULL);

	if (lp == NULL) {
		cerr << "Error leyendo el LP en: " << ruta << "." << endl;
	}
} /* levanto instancia *.MPS, *.LP o *.SAV */


void problemaCPLEX::setearTiempoMaximo(double limite_en_segundos)
{
    status = CPXsetdblparam(env, CPX_PARAM_TILIM, limite_en_segundos);
    if (status) {
        cerr << "Problema seteando el tiempo maximo de ejecucion." << endl;
    }
} /* asigno un tiempo maximo de ejecucion */


void problemaCPLEX::elegirEstrategiaDeSeleccionDeNodo()
{
	/* best-bound: se elige el nodo de mayor optimo en la relajacion */
	status = CPXsetintparam(env, CPX_PARAM_NODESEL, CPX_NODESEL_BESTBOUND);
}


void problemaCPLEX::elegirEstrategiaDeSeleccionDeVariable()
{
	/* minimum infeasibility: elige la variable de valor mas cercano a un entero */
	status = CPXsetintparam(env, CPX_PARAM_VARSEL, CPX_VARSEL_MININFEAS);
}


void problemaCPLEX::asignarOptimoDeRelajacion(double valor)
{
	hayRelajacionEnRaiz = true;
	relajacionEnRaiz = valor;
} /* para asignar el valor del optimo en el nodo raiz en la instancia */


void problemaCPLEX::configuracion(bool bc, bool cb, bool cl, bool gCo, bool eCo)
{
	// aviso a cplex informacion de cortes
	status = CPXsetusercutcallbackfunc(env, planosDeCorte, this);
	if (status)
	{
		fprintf(stderr, "Falla en CPXsetusercutcallbackfunc().\n");
	}

	// decido tecnica algoritmica
	if ( not(bc or cb) or not(cl or gCo or eCo) )
	{
		/* branch and bound, no usa cortes */
		return;
	}

	double time_beg = 0.0;
	double time_end = 0.0;

	status = CPXgettime(env,&time_beg);

	if (bc)
		tipo = BRANCH_AND_CUT;
	else
		tipo = CUT_AND_BRANCH;

	int numR = numeroRestricciones();
	int numV = numeroVariables();

	// preparo estructuras para cortes
	if (gCo or eCo or cl)
	{
		// agarro informacion de restricciones del MIP
		char senses[numR];
		status = CPXgetsense(env, lp, senses, 0, numR-1);
		if (status) { return; }

		vector< vector<double> > desigualdades(numR);
		vector< vector<int> > subindices(numR);
		vector<double> bes(numR, 0.0);

		for (int r = 0; r < numR; r++)
		{
			char sentido = senses[r];
			if ((sentido == 'L') or (sentido == 'G'))
			{
				dameRestriccion(r, desigualdades[r], subindices[r]);
				if (status) { return; }

				bes[r] = dameRhs(r);
				if (status) { return; }

				if (sentido == 'G')
				{
					bes[r] = -1.0*bes[r];
					for (int j = 0; j < desigualdades[r].size(); j++)
					{
						desigualdades[r][j] = -1.0 * desigualdades[r][j];
					}
					senses[r] = 'L';
				}
			}
		}

		// preprocesamiento necesario para cortes cover
		if (gCo or eCo)
		{
			mochilas.reajustar(numR, numV, gCo, eCo);
			agregarMochilas(numR, senses, desigualdades, subindices, bes);
		}

		// preprocesamiento necesario para cortes clique
		if (cl)
		{
			grafoDeConflictos.ingresarCantidadDeNodos(numV);
			armarGrafoDeConflictos(numR, senses, desigualdades, subindices, bes);
		}
	}

	status = CPXgettime(env,&time_end);
	tiempoPreOptimizacion = time_end - time_beg;
} /* preparar los cortes y el algoritmo elegidos */


// agregado de un corte ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int problemaCPLEX::agregarCorte
					(CPXCENVptr env, void* cbdata, int wherefrom, int& estado,
					 const vector<double>& corte, const vector<int>& indice, double rhs)
{
	int nzcnt = corte.size();

	if (nzcnt > 0)
	{
		double cutval[nzcnt];
		int cutind[nzcnt];

		for (int j = 0; j < nzcnt; j++)
		{
			cutval[j] = corte[j];
			cutind[j] = indice[j];
		}

		estado = CPXcutcallbackadd(env, cbdata, wherefrom, nzcnt, rhs, 'L', 
									cutind, cutval, CPX_USECUT_FORCE);

		return 1;
	}

	return 0;
} /* para agregar un corte encontrado con desigualdades cover o clique */


// cortes covers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool problemaCPLEX::sePidieronCortesCover() const
{
	return not(mochilas.estaVacio());
}


int problemaCPLEX::agregarCortesCover
	(CPXCENVptr env,void* cbdata,int wherefrom, int& estado, const double* x, int tamanho)
{
	int cortesNuevos = 0;
	int restricciones = numeroRestricciones();

	// trato de encontrar un cover en cada restriccion original del MIP
	for (int r = 0; r < restricciones; r++)
	{
		if (mochilas.puedoBuscarEnRestriccion(r))
		{
			vector<double> corte;
			vector<int> indice;
			double rhs;
			mochilas.buscarCover(r, x, tamanho, corte, indice, rhs);

			cortesNuevos += agregarCorte
							(env, cbdata, wherefrom, estado, corte, indice, rhs);

			if (estado){
				fprintf(stderr,"Falla en agregar corte cover.\n");
				return 0;
			}
		}
	}

	return cortesNuevos;
} /* busca cortes cover y devuelve cuantos agrego */


// cortes clique ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool problemaCPLEX::sePidieronCortesClique() const
{
	return not(grafoDeConflictos.grafoVacio());
}


int problemaCPLEX::agregarCortesClique
	(CPXCENVptr env,void* cbdata,int wherefrom, int& estado, const double* x, int tamanho)
{

	int cortesNuevos = 0;
	int cortePorNodo = 0;
	double acum = 0.0;

	// ordeno los valores de x* de menor a mayor
	vector<bool> agregados(tamanho, false);
	vector<double> ordX(tamanho,0.0);
	vector<int> ordI(tamanho,0);
	forn(i,tamanho)
	{
		ordX[i] = x[i];
		ordI[i] = i;
		acum += x[i];
	}
	mergeSort(ordI,ordX);

	// por cada nodo busco una clique
	rforn(i, tamanho)
	{
		if (agregados[i])
			continue;

		int nodo = ordI[i];
		vector<double> corte;
		vector<int> indice;
		double rhs;
		grafoDeConflictos.buscarClique(nodo, x, tamanho, corte, indice, rhs);

		cortePorNodo = agregarCorte
						(env, cbdata, wherefrom, estado, corte, indice, rhs);

		if (estado){
			fprintf(stderr,"Falla en agregar corte cover.\n");
			return 0;
		}

		if (cortePorNodo == 1)
		{
			forn(k, corte.size())
			{
				agregados[indice[k]] = true;
				acum -= x[indice[k]];
			}

			cortesNuevos += 1;
		}

		if (acum < 0.5)
			break;
	}

	return cortesNuevos;
} /* busca cortes clique y devuelve cuantos agrego */



/** PRIVATE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// para leer restricciones del problema original ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void problemaCPLEX::dameRestriccion(int r, vector<double>& fila, vector<int>& indices)
{
	int rmatbeg[1];
	int nzcnt, surplus;
	status = CPXgetrows(env, lp, &nzcnt, rmatbeg, NULL, NULL, 0, &surplus, r, r);
	if ( (status != CPXERR_NEGATIVE_SURPLUS) and (status != 0) ) {
		fprintf(stderr,"Error al obtener parametros de la fila %d.\n",r);
		return;
	}

	int rmatspace = abs(surplus);
	double rmatval[rmatspace];
	int rmatind[rmatspace];

	status = CPXgetrows(env, lp, &nzcnt, rmatbeg, rmatind,
						rmatval, rmatspace, &surplus, r, r);
	if (status){
		fprintf(stderr,"Error al obtener la fila %d.\n",r);
	}

	fila.resize(nzcnt,0.0);
	indices.resize(nzcnt,0);

	for (int i = 0; i < rmatspace; i++)
	{
		fila[i] = rmatval[i];
		indices[i] = rmatind[i];
	}
} /* obtiene la restriccion r-esima del problema original */


double problemaCPLEX::dameRhs(int r)
{
	double b[1];
	status = CPXgetrhs(env, lp, b, r, r);
	if (status){
		fprintf(stderr,"Error al obtener el RHS de la fila %d.\n",r);
	}
	return b[0];
} /* obtiene el RHS r-esimo del problema original */


// guardo restricciones en Covers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void problemaCPLEX::agregarMochilas
					(int numR,const char* senses, 
					const vector< vector<double> >& desigualdades,
					const vector< vector<int> >& subindices,
					const vector<double>& bes)
{
	for (int r = 0; r < numR; r++)
	{
		if (senses[r] == 'L')
		{
			mochilas.agregarRestriccionesTraducidas
				(r, desigualdades[r], subindices[r], bes[r]);
		}
	}

} /* traduzco restricciones a desigualdades mochila y las guardo */


// preparo el grafo de conflictos ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void problemaCPLEX::armarGrafoDeConflictos
					(int numR,const char* senses, 
					const vector< vector<double> >& desigualdades,
					const vector< vector<int> >& subindices,
					const vector<double>& bes)
{
	// primera fase de busqueda de ejes
	for (int r = 0; r < numR; r++)
	{
		if (senses[r] == 'L')
		{
			grafoDeConflictos.buscarEjesEnRestriccion
				(desigualdades[r],subindices[r],bes[r]);
		}
	}

	// si no encontre ejes con primera fase no puedo buscar en segunda fase
	if (grafoDeConflictos.cuantosEjes() == 0)
		return;

	// segunda fase de busqueda de ejes
	while (true)
	{
		int ejes = grafoDeConflictos.cuantosEjes();

		for (int r = 0; r < numR; r++)
		{
			if (senses[r] == 'L')
			{
				grafoDeConflictos.buscarConCliqueEnRestriccion
								(desigualdades[r],subindices[r],bes[r]);
			}
		}

		if (grafoDeConflictos.cuantosEjes() - ejes == 0)
			break;
	}
} /* armo grafo de conflictos y lo guardo */



/** CUT CALLBACK ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool estoyEnRaiz(CPXCENVptr env, void* cbdata, int wherefrom, int& estado)
{
	int nroNodo;
	estado = CPXgetcallbackinfo
				(env, cbdata, wherefrom, CPX_CALLBACK_INFO_NODE_COUNT, &nroNodo);

	if (estado)
		fprintf(stderr,"Error al pedir informacion de nodo.\n");

	return (nroNodo==0);
} /* para saber si el callback vuelve de analizar al nodo raiz */


int CORTES_AGREGADOS = 0;
int VECES_PROCESAMIENTO_NODO = 0;


static int CPXPUBLIC 
	planosDeCorte
		(CPXCENVptr env, void *cbdata, int wherefrom, void *cbhandle, int *useraction_p)
{
//// preambulo al cut callback
	int estado = 0, addcuts = 0;

	problemaCPLEX *problema = (problemaCPLEX*)cbhandle;
	enum ALGORITMO probType = problema->elTipo();
	bool soloEnRaiz = ( probType != BRANCH_AND_CUT );
	bool esRaiz = estoyEnRaiz(env, cbdata, wherefrom, estado);
	*useraction_p = CPX_CALLBACK_ABORT_CUT_LOOP;

	if (estado or (soloEnRaiz and not(esRaiz)))
		return estado;

	if (esRaiz)
	{
		// en la raiz soy mas violento con cortes y veces de procesamiento
		if (VECES_PROCESAMIENTO_NODO > 20 or CORTES_AGREGADOS > 10000)
		{
			CORTES_AGREGADOS = 0;
			VECES_PROCESAMIENTO_NODO = 0;
			return estado;
		}
		if (not problema->optimoDeRelajacionAgregado())
		{
			double optimoRelajacion = 0.0;
			estado = CPXgetcallbacknodeinfo(env, cbdata, wherefrom, 0,
							CPX_CALLBACK_INFO_NODE_OBJVAL, &optimoRelajacion);
			problema->asignarOptimoDeRelajacion(optimoRelajacion);
		}
	}
	else
	{
		// en nodos no raiz hago un unico procesamiento
		if (VECES_PROCESAMIENTO_NODO > 0)
		{
			CORTES_AGREGADOS = 0;
			VECES_PROCESAMIENTO_NODO = 0;
			return estado;
		}
	}

	if (probType == BRANCH_AND_BOUND)
		return estado;

	int tamanho = problema->numeroVariables();
	double x[tamanho];
	estado = CPXgetcallbacknodex(env, cbdata, wherefrom, x, 0, tamanho-1);
	if (estado) {
		fprintf(stderr, "Problema obteniendo x* en planosDeCorte.\n");
		return estado;
	}

	double time_beg = 0.0;
	double time_end = 0.0;

	estado = CPXgettime(env,&time_beg);

//// cortes cover
	if (problema->sePidieronCortesCover())
	{
		addcuts += (problema->agregarCortesCover
					(env, cbdata, wherefrom, estado, x, tamanho));

		if (estado)
			return (estado);
	}


//// cortes clique
	if (problema->sePidieronCortesClique())
	{
		addcuts += (problema->agregarCortesClique
					(env, cbdata, wherefrom, estado, x, tamanho));

		if (estado)
			return (estado);
	}


//// preparo salida avisandole a CPLEX si se crearon o no cortes
	if ( addcuts > 0 )
	{
		CORTES_AGREGADOS += addcuts;
		VECES_PROCESAMIENTO_NODO += 1;
		*useraction_p = CPX_CALLBACK_SET; 
	}
	else
	{
		CORTES_AGREGADOS = 0;
		VECES_PROCESAMIENTO_NODO = 0;
	}

	estado = CPXgettime(env,&time_end);
	problema->sumarTiempoDeCallback(time_end - time_beg);

	return (estado);
} /* cut callback para CPLEX */
