#ifndef _PROBLEMACPLEX_H
#define _PROBLEMACPLEX_H

#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>

class problemaCPLEX
{
public:
	problemaCPLEX();

	void deshabilitarParametros();
	void leerLP(const char* ruta);
	

private:
	int status;
	CPXENVptr env;	//Puntero al entorno
	CPXLPptr lp;	//Puntero al lp

	CPLXENVptr generarEntorno();
};

#endif // _PROBLEMACPLEX_H
