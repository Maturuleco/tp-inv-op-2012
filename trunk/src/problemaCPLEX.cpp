#include "problemaCPLEX.hpp"

// constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
problemaCPLEX::problemaCPLEX()
:status(0), lp(NULL)
{
	env = CPXopenCPLEX(&status);

	if (env == NULL) {
		cerr << "Error creando el entorno." << endl;
	}
}


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
}


// para errores ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool problemaCPLEX::hayError() const
{
	return (status != 0);
}


void problemaCPLEX::mostrarError() const
{
	char errmsg[CPXMESSAGEBUFSIZE];
	CPXCCHARptr translation = CPXgeterrorstring (env, status, errmsg);
	if (translation != NULL){
		fprintf (stderr, "%s", errmsg);
	} else {
		fprintf (stderr, "CPLEX ERROR %d.\n", status);
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double problemaCPLEX::optimo()
{
    double objval;
    status = CPXgetobjval(env, lp, &objval);

    if (status) {
        cerr << "Problema obteniendo mejor solución entera." << endl;
    }
    return objval;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void problemaCPLEX::deshabilitarParametros()
{
	status = CPXsetintparam(env, CPX_PARAM_PREIND, 0);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_PRELINEAR, 0);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_EACHCUTLIM, 0);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_CUTPASS, 0);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_FRACCUTS, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_HEURFREQ, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_RINSHEUR, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_REDUCE, 0);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_IMPLBD, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_MCFCUTS, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_ZEROHALFCUTS, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_MIRCUTS, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_GUBCOVERS, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_FLOWPATHS, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_FLOWCOVERS, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_DISJCUTS, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_COVERS, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_CLIQUES, -1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_THREADS, 1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_MIPSEARCH, 1);
	if (status) return;
	status = CPXsetintparam(env, CPX_PARAM_MIPCBREDLP, CPX_OFF);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void problemaCPLEX::leerLP(const char* ruta)
{
	lp = CPXcreateprob (env, &status, ruta);

	if (lp == NULL) {
		cerr << "Error leyendo el LP en: " << ruta << "." << endl;
		return;
	}

	status = CPXreadcopyprob(env, lp, ruta, NULL);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void problemaCPLEX::setearTiempoMaximo(double limite)
{
    status = CPXsetdblparam(env, CPX_PARAM_TILIM, limite);
    if (status) {
        cerr << "Problema seteando el tiempo maximo de ejecucion." << endl;
    }
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void problemaCPLEX::resolverMIP()
{
	status = CPXmipopt (env, lp);
	if ( status ) {
		fprintf (stderr, "Failed to optimize MIP.\n");
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void problemaCPLEX::mostrarSolucion()
{
	int solstat = CPXgetstat (env, lp);
	printf ("Solution status %d.\n", solstat);

	double objVal = optimo();
	printf ("Objective value %.10g\n", objVal );

	int cur_numcols = CPXgetnumcols (env, lp);
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
		if ( fabs (variables[j]) > 1e-10 ) {
			printf ( "Column %d:  Value = %17.10g\n", j, variables[j]);
		}
	}
}
