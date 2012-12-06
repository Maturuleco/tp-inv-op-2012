#ifndef _PROBLEMACPLEX_H_
#define _PROBLEMACPLEX_H_

// cplex callable library ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>
ILOSTLBEGIN


// modulos de c o c++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <vector>
#include <iostream>
#include "covers.hpp"
#include <math.h>
using namespace std;


// distinguir algoritmo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
enum ALGORITMO {BRANCH_AND_BOUND, BRANCH_AND_CUT, CUT_AND_BRANCH};


// funcion callback para CPLEX ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int CPXPUBLIC
	planosDeCorte	(CPXCENVptr env,		// puntero a environment de CPLEX
					void *cbdata,			// puntero necesario para llamar otras rutinas
					int wherefrom,			// indica callback type, CPX_CALLBACK_MIP_CUT
					void *cbhandle,			// puntero a informacion de usuario
					int *useraction_p);		/* digo que hacer despues de rutina:
											   CPX_CALLBACK_DEFAULT cortes agregados
											   CPX_CALLBACK_FAIL terminar optimizacion
											   CPX_CALLBACK_ABORT_LOOP_CUT hace branching
											   CPX_CALLBACK_SET cortes agregados */
	/** la funcion devuelve 0 si hubo exito, o nonzero si hubo error */


// definicion ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class problemaCPLEX {

public:

	// class
	problemaCPLEX();
	~problemaCPLEX();

	// observadores
	bool hayError() const;
	void mostrarError() const;
	enum ALGORITMO elTipo() const;
	int numeroVariables() const;
	int numeroRestricciones() const;

	// setters
    double getTime();
	double optimo();
    void leerLP(const char* ruta);
    void deshabilitarParametros();
	void elegirEstrategiaDeSeleccionDeNodo();
	void elegirEstrategiaDeSeleccionDeVariable();
	void configuracion(bool bc, bool cb, bool cl, bool co);
    void setearTiempoMaximo(double limite_en_segundos);
	void mostrarSolucion();
	void resolverMIP();

	// cortes cover
	bool sePidieronCortesCover();
	int agregarCortesCover(CPXCENVptr,void*,int,void*,int&,const double*,int);

private:

	// preparacion de cortes cover (pre optimizacion)
	void dameRestriccion(int, vector<double>&, vector<int>&);
	double dameRhs(int);
	int agregarMochilas();

	// preparacion cortes clique (pre optimizacion)
	int armarGrafoDeConflictos();

	// representacion interna
	double tiempoDeOptimizacion;
	int numeroDeNodosDeOptimizacion;
	int status;
	CPXENVptr env;	//Puntero al entorno
	CPXLPptr lp;	//Puntero al lp
	enum ALGORITMO tipo;
	Covers mochilas;
};


#endif // _PROBLEMACPLEX_H_
