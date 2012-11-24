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


// definicion ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class problemaCPLEX {

public:

	// class
	problemaCPLEX();
	~problemaCPLEX();

	bool hayError() const;
	void mostrarError() const;

	// setters
    void deshabilitarParametros();
    void leerLP(const char* ruta);
	void resolverMIP();
    void setearTiempoMaximo(double limite);		// limite en segundos
	double optimo();
	void mostrarSolucion();

	// son necesarias?
    double getTime();
    void escribirLP(const char* ruta);


private:

	int status;
	CPXENVptr env;	//Puntero al entorno
	CPXLPptr lp;	//Puntero al lp

};


#endif // _PROBLEMACPLEX_H
