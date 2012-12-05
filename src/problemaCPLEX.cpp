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


void problemaCPLEX::configuracion(bool bc, bool cb, bool cl, bool co)
{
	if ( not(bc or cb) or not(cl or co) ) {
		/* branch and bound, no usa cortes */
		return;
	}

	if (co)		/* cortes cover */
	{
		status = agregarMochilas();
		if (status) {
			fprintf(stderr,"Falla al preparar mochilas.\n");
			return;
		}
	}

	if (cl)		/* cortes clique */
	{
		status = armarGrafoDeConflictos();
		if (status) {
			fprintf(stderr,"Falla al armar grafo de conflictos.\n");
			return;
		}
	}

	if (bc) {
		tipo = BRANCH_AND_CUT;
	} else {
		tipo = CUT_AND_BRANCH;
	}

	status = CPXsetusercutcallbackfunc(env, planosDeCorte, this);
	if (status) {
		fprintf(stderr, "Falla en CPXsetcutcallbackfunc().\n");
	}
} /* preparar los cortes y el algoritmo elegidos */


// optimizacion ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void problemaCPLEX::resolverMIP()
{
	status = CPXmipopt (env, lp);
	if ( status ) {
		fprintf (stderr, "Failed to optimize MIP.\n");
	}
} /* le avisamos a CPLEX que busque el optimo */


void problemaCPLEX::mostrarSolucion()
{
	double tolerancia = 1e-10;

	int solstat = CPXgetstat (env, lp);
	printf ("Solution status %d.\n", solstat);

	double objVal = optimo();
	printf ("Objective value %.10g\n", objVal );

	int cur_numcols = numeroVariables();
	double variables[cur_numcols];

	for (int i = 0; i < cur_numcols; i++){
		variables[i] = 0.0;
	}

	double * x = (double*)&variables;
	status = CPXgetx (env, lp, x, 0, cur_numcols-1);
	if ( status ) {
		fprintf (stderr, "Failed to obtain solution.\n");
		return;
	}

	for (int j = 0; j < cur_numcols; j++) {
		if ( fabs (variables[j]) > tolerancia ) {
			printf ( "Column %d:  Value = %17.10g\n", j, variables[j]);
		}
	}
	printf("Cortes Cover Greedy Agregados> %d\n", mochilas.cuantosGreedy());
	printf("Cortes Cover Dynamic Agregados> %d\n", mochilas.cuantosDinamicos());
} /* mostrar por stdout los resultados obtenidos */


// cortes covers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool problemaCPLEX::sePidieronCortesCover()
{
	return not(mochilas.estaVacio());
}


int problemaCPLEX::agregarCortesCover
	(CPXCENVptr env,void* cbdata,int wherefrom, void* cbhandle,
	int& estado, const double* x, int tamanho)
{
	int cortesNuevos = 0;
	int restricciones = numeroRestricciones();

	for (int r = 0; r < restricciones; r++)
	{
		if (mochilas.puedoBuscarEnRestriccion(r))
		{
			vector<double> corte;
			vector<int> indices;
			double rhs;
			mochilas.buscarCover(r, x, tamanho, corte, indices, rhs);
			int nzcnt = corte.size();

			if (nzcnt > 0)
			{
				double cutval[nzcnt];
				int cutind[nzcnt];

				for (int j = 0; j < nzcnt; j++)
				{
					cutval[j] = corte[j];
					cutind[j] = indices[j];
				}

				estado = CPXcutcallbackadd(env, cbdata, wherefrom, nzcnt, rhs, 'L', 
											cutind, cutval, CPX_USECUT_FORCE);

				if (estado){
					fprintf(stderr,"Falla en agregar corte cover.\n");
					return 0;
				}

				cortesNuevos += 1;
			}
		}
	}

	return cortesNuevos;
} /* busca cortes cover y devuelve cuantos agrego */



/** PRIVATE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// para covers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void problemaCPLEX::dameRestriccion(int r, vector<double>& fila, vector<int>& indices)
{
	int rmatbeg[1];
	int nzcnt, surplus;
	status = CPXgetrows(env, lp, &nzcnt, rmatbeg, NULL, NULL, 0, &surplus, r, r);
	if ( (status != CPXERR_NEGATIVE_SURPLUS) and (status != 0) ) {
		fprintf(stderr,"Error al obtener la fila %d.\n",r);
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


int problemaCPLEX::agregarMochilas()
{
	int numR = numeroRestricciones();
	int numV = numeroVariables();
	if (numV*numR == 0) { return 1; }

	mochilas.reajustar(numR,numV);
	char senses[numR];

	status = CPXgetsense(env, lp, senses, 0, numR-1);
	if (status) { return (status); }

	for (int r = 0; r < numR; r++)
	{
		char sentido = senses[r];
		if ((sentido == 'L') or (sentido == 'G'))
		{
			vector<double> fila;
			vector<int> indices;
			dameRestriccion(r, fila, indices);
			if (status) { return (status); }

			double b = dameRhs(r);
			if (status) { return (status); }

			if (sentido == 'G')
			{
				b = -1.0*b;
				for (int j = 0; j < fila.size(); j++)
				{
					fila[j] = -1.0*fila[j];
				}
			}
			mochilas.agregarRestriccionesTraducidas(r, fila, indices, b);
		}
	}

	return status;
} /* traduzco restricciones a desigualdades mochila y las guardo */


// para cliques ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int problemaCPLEX::armarGrafoDeConflictos()
{
	return 0;
} /* armo grafo de conflictos y lo guardo */



/** CUT CALLBACK ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool estoyEnRaiz(CPXCENVptr env, void* cbdata, int wherefrom, int& estado)
{
	int nroNodo;
	estado = CPXgetcallbackinfo
				(env, cbdata, wherefrom, CPX_CALLBACK_INFO_NODE_COUNT, &nroNodo);

	if (estado){
		fprintf(stderr,"Error al pedir informacion de nodo.\n");
	}

	return (nroNodo==0);
} /* para saber si el callback vuelve de analizar al nodo raiz */


int LIMITE_PARA_BRANCHING = 0;


static int CPXPUBLIC 
	planosDeCorte
		(CPXCENVptr env, void *cbdata, int wherefrom, void *cbhandle, int *useraction_p)
{
//// preambulo al cut callback
	int estado = 0, addcuts = 0;

	problemaCPLEX *problema = (problemaCPLEX*)cbhandle;
	bool soloEnRaiz = problema->elTipo() == CUT_AND_BRANCH;
	bool esRaiz = estoyEnRaiz(env, cbdata, wherefrom, estado);
	*useraction_p = CPX_CALLBACK_ABORT_CUT_LOOP;
	
	if (estado or (soloEnRaiz and not(esRaiz)))
	{
		return estado;
	}

	if (LIMITE_PARA_BRANCHING > 10)
	{
		LIMITE_PARA_BRANCHING = 0;
		return estado;
	}

	int tamanho = problema->numeroVariables();
	double x[tamanho];
	estado = CPXgetcallbacknodex(env, cbdata, wherefrom, x, 0, tamanho-1);
	if (estado) {
		fprintf(stderr, "Problema obteniendo x* en planosDeCorte.\n");
		return estado;
	}


//// cortes cover
	if (problema->sePidieronCortesCover())
	{
		addcuts += (problema->agregarCortesCover
					(env,cbdata,wherefrom,cbhandle, estado, x, tamanho));
		if (estado){
			return (estado);
		}
	}


//// preparo salida avisandole a CPLEX si se crearon o no cortes
	if ( addcuts > 0 ) {
		LIMITE_PARA_BRANCHING += addcuts;
		*useraction_p = CPX_CALLBACK_SET; 
	}

	return (estado);
} /* cut callback para CPLEX */
