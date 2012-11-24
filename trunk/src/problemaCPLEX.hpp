#ifndef _PROBLEMACPLEX_H
#define _PROBLEMACPLEX_H

// cplex callable library ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>
ILOSTLBEGIN


// modulos de c o c++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <vector>
#include <iostream>
using namespace std;


class problemaCPLEX
{
public:
	problemaCPLEX();

	double getObjVal();
    double getTime();
    void deshabilitarParametros();
    void leerLP(const char* ruta);
    void escribirLP(const char* ruta);
    void setearTiempoMaximo(const int limite);

private:
	int status;
	CPXENVptr env;	//Puntero al entorno
	CPXLPptr lp;	//Puntero al lp

	void generarEntorno();
};

#endif // _PROBLEMACPLEX_H
