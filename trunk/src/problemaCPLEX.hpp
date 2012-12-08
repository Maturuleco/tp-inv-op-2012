/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Clase para intermediar entre el CPLEX y modulos propios.
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __PROBLEMACPLEX_H__
#define __PROBLEMACPLEX_H__

// modulos propios ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "aux.hpp"
#include "grafo.hpp"
#include "covers.hpp"


// cplex callable library ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>
ILOSTLBEGIN


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
	bool sePidieronCortesCover() const;
	int agregarCortesCover(CPXCENVptr,void*,int,void*,int&,const double*,int);

	// cortes clique
	bool sePidieronCortesClique() const;

private:

	// preparacion de cortes cover (pre optimizacion)
	void dameRestriccion(int, vector<double>&, vector<int>&);
	double dameRhs(int);
	int agregarMochilas();

	// preparacion cortes clique (pre optimizacion)
	int armarGrafoDeConflictos();

	// representacion interna
	double tiempoDeOptimizacion;		/* tiempo de CPXmipopt */
	int numeroDeNodosDeOptimizacion;	/* nodos usados en CPXmipopt */
	int status;							/* status de rutinas de CPLEX */
	CPXENVptr env;						/* entorno del MIP */
	CPXLPptr lp;						/* el prbolema MIP */
	enum ALGORITMO tipo;				/* tipo de algoritmo de resolucion */
	Covers mochilas;					/* desigualdades mochila del MIP */
	Grafo grafoDeConflictos;			/* grafo de conflictos a partir de restriccion */
};


#endif // __PROBLEMACPLEX_H__