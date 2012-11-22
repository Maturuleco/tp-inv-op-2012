#include "problemaCPLEX.hpp"

problemaCPLEX::problemaCPLEX()
:status(0), lp(NULL)
{
	env = generarEntorno();
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
	this->env = CPXopenCPLEX(&this->status);

	if (this->env == NULL) {
		cerr << "Error creando el entorno" << endl;
		exit(1);
	}
}

void problemaCPLEX::leerLP(const char* ruta)
{
	char probname[16];
	strcopy (probname, ruta); // TODO: Mejorar esto

	this->lp = CPXcreateprob (this->env, &this->status, probname);

	this->status = CPXreadcopymipstart(this->env, this->lp, ruta);

	if (this->lp == NULL) {
		cerr << "Error creando el LP" << endl;
		exit(1);
	}
	return lp;
}
