/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 'Covers' permite guardar restricciones normalizandolas a mochila (solo sirve para 
 * problemas con variables binarias) y buscar un plano de corte con una solucion de la
 * relajacion lineal.
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __COVERS_H__
#define __COVERS_H__

#include "aux.hpp"


// definicion ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class Covers {

public:

	// class
	Covers();
	void reajustar(int,int);

	// getters
	bool estaVacio() const;
	int numeroRestricciones() const;
	int numeroVariables() const;
	bool puedoBuscarEnRestriccion(int) const;
	int cuantosGreedy() const;
	int cuantosDinamicos() const;

	// setters
	void buscarCover(int,const double*,int,vector<double>&,vector<int>&,double&);
	void agregarRestriccionesTraducidas
			(int, const vector<double>&, const vector<int>&, double);

private:

	bool resolverMochila(int,const vector<double>&,vector<bool>&);
	bool resolverMochilaGreedy(int,const vector<double>&,vector<bool>&) const;
	bool resolverMochilaDinamica(int,const vector<double>&,vector<bool>&) const;

	int cant_variables;
	int cant_restricciones;
	int coversGreedyAgregados;
	int coversDinamicosAgregados;

	vector< vector<double> > restricciones;	// guarda coeficientes no nulos
	vector< vector<bool> > indicadores;		// indica si es una variable complemento
	vector< vector<int> > indices;			// indica el indice de la variable
	vector<bool> validas;					// dice si la la restriccion es usable
	vector<double> rhs;						// guarda el rhs de la restriccion
};


#endif // __COVERS_H__
