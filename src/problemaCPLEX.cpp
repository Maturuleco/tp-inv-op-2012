#include "problemaCPLEX.hpp"

// constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
problemaCPLEX::problemaCPLEX()
:status(0), lp(NULL)
{
	generarEntorno();
}



double problemaCPLEX::getObjVal()
{
    double objval;
    this->status = CPXgetobjval(this->env, this->lp, &objval);
    // TODO: definir si el chequeo del status se hace en esta clase o afuera
    // por ahora lo pongo en esta clase
    if (status) {
        cerr << "Problema obteniendo mejor solución entera" << endl;
        exit(1);
    }
    return objval;
}


double problemaCPLEX::getTime()
{
    double time;
    this->status = CPXgettime(this->env, &time);
    return time;
}


void problemaCPLEX::deshabilitarParametros()
{
	this->status = CPXsetintparam(this->env, CPX_PARAM_PREIND, 0);
	this->status = CPXsetintparam(this->env, CPX_PARAM_PRELINEAR, 0);
	this->status = CPXsetintparam(this->env, CPX_PARAM_EACHCUTLIM, 0);
	this->status = CPXsetintparam(this->env, CPX_PARAM_CUTPASS, 0);
	this->status = CPXsetintparam(this->env, CPX_PARAM_FRACCUTS, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_HEURFREQ, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_RINSHEUR, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_REDUCE, 0);
	this->status = CPXsetintparam(this->env, CPX_PARAM_IMPLBD, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_MCFCUTS, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_ZEROHALFCUTS, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_MIRCUTS, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_GUBCOVERS, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_FLOWPATHS, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_FLOWCOVERS, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_DISJCUTS, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_COVERS, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_CLIQUES, -1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_THREADS, 1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_MIPSEARCH, 1);
	this->status = CPXsetintparam(this->env, CPX_PARAM_MIPCBREDLP, CPX_OFF);
}


void problemaCPLEX::generarEntorno()
{
	env = CPXopenCPLEX(&status);

	if (env == NULL) {
		cerr << "Error creando el entorno" << endl;
		exit(1);
	}
}


void problemaCPLEX::leerLP(const char* ruta)
{
	char probname[16];
    strcpy (probname, ruta); // TODO: Mejorar esto

	this->lp = CPXcreateprob (this->env, &this->status, probname);

	this->status = CPXreadcopyprob(this->env, this->lp, ruta, NULL); //TODO: revisar esto

	if (this->lp == NULL) {
		cerr << "Error creando el LP" << endl;
		exit(1);
	}
}


void problemaCPLEX::escribirLP(const char* ruta)
{
    this->status = CPXwriteprob(this->env, this->lp, ruta, NULL);
    if (status) {
        cerr << "Problema escribiendo el modelo" << endl;
        exit(1);
    }
}


void problemaCPLEX::setearTiempoMaximo(const int limite)
{
    this->status = CPXsetdblparam(this->env, CPX_PARAM_TILIM, limite);
    if (status) {
        cerr << "Problema seteando el tiempo maximo de ejecucion" << endl;
        exit(1);
    }
}
