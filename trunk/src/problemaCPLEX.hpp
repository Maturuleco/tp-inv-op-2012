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
	bool optimoDeRelajacionAgregado() const;
	int tipoDeOptimizacion() const;

	// setters
    double getTime();
	double optimo();
    void leerLP(const char* ruta);
    void deshabilitarParametros();
	void elegirEstrategiaDeSeleccionDeNodo();
	void elegirEstrategiaDeSeleccionDeVariable();
	void configuracion(bool, bool, bool, bool, bool);
    void setearTiempoMaximo(double limite_en_segundos);
	void asignarOptimoDeRelajacion(double);
	void sumarTiempoDeCallback(double);
	void mostrarSolucion();
	void resolverMIP();

	// agregado de cortes
	int agregarCorte(CPXCENVptr, void*, int, int&,
					  const vector<double>&, const vector<int>&, double rhs);

	// cortes cover
	bool sePidieronCortesCover() const;
	int agregarCortesCover(CPXCENVptr,void*,int,int&,const double*,int);

	// cortes clique
	bool sePidieronCortesClique() const;
	int agregarCortesClique(CPXCENVptr,void*,int,int&,const double*,int);

private:

	// para leer restriccion del MIP
	void dameRestriccion(int, vector<double>&, vector<int>&);
	double dameRhs(int);

	// preparacion de cortes cover (pre optimizacion)
	void agregarMochilas(int,const char*, const vector< vector<double> >&,
						const vector< vector<int> >&, const vector<double>&);

	// preparacion cortes clique (pre optimizacion)
	void armarGrafoDeConflictos(int,const char*, const vector< vector<double> >&,
							   const vector< vector<int> >&, const vector<double>&);

	// representacion interna
	bool hayRelajacionEnRaiz;			/* indica si se seteo valor de relajacionEnRaiz */
	double relajacionEnRaiz;			/* valor optimo en la relajacion del nodo raiz */
	double tiempoPreOptimizacion;		/* tiempo en preparacion para cortes */
	double tiempoDeOptimizacion;		/* tiempo de CPXmipopt */
	double tiempoDeCallback;			/* tiempo total de busquda de cortes */
	int status;							/* status de rutinas de CPLEX */
	CPXENVptr env;						/* entorno del MIP */
	CPXLPptr lp;						/* el prbolema MIP */
	enum ALGORITMO tipo;				/* tipo de algoritmo de resolucion */
	Covers mochilas;					/* desigualdades mochila del MIP */
	Grafo grafoDeConflictos;			/* grafo de conflictos a partir de restriccion */
};


#endif // __PROBLEMACPLEX_H__
