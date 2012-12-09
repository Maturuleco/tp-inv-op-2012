/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 'Grafo' guarda el grafo de conflictos encontrado a partir de las restricciones.
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __GRAFO_H__
#define __GRAFO_H__

#include "aux.hpp"

// definicion ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class Grafo {

public:

	// constructor
	Grafo();
	void ingresarCantidadDeNodos(int);

	// getters
	void mostrar() const;
	bool sonVecinos(int,int,bool c=false,bool d=false) const;
	bool grafoVacio() const;
	bool cuantosCortes() const;
	int cuantosEjes() const;
	int particionarEnCliques(const vector<int>&,const vector<int>&,
							 const vector<bool>&,vector<int>&) const;

	// setters
	void agregarEje(int,int,bool c=false, bool d=false);
	void buscarEjesEnRestriccion(const vector<double>&,const vector<int>&,double);
	void buscarConCliqueEnRestriccion(const vector<double>&,const vector<int>&,double);
	void buscarClique(int,const double*,int,vector<double>&,vector<int>&,double&);

private:

	/* ACLARACIONES:
	 * - los nodos [0..n-1] son originales, los nodos [n..2*n-1] son complemento.
	 * - no agregamos los ejes entre un nodo y su complemento.
	 */

	int numeroNodos;
	int numeroEjes;
	int numeroCortes;
	vector< list<int> > graph;
};


#endif // __GRAFO_H__
