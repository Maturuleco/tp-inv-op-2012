/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 'Grafo' guarda el grafo de conflictos encontrado a partir de las restricciones.
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __GRAFO_H__
#define __GRAFO_H__

#include "aux.hpp"


// macros ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define forn(i,n) for(int i = 0; i < (int)(n); i++)
#define forkn(i,k,n) for(int i = (int)(k); i < (int)(n); i++)
#define rforn(i,n) for(int i = (int)(n-1); i >= 0; i--)


// definicion ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class Grafo {

public:

	// constructor
	Grafo();
	void ingresarCantidadDeNodos(int);

	// getters
	bool grafoVacio() const;
	int particionarEnCliques(const vector<double>&,const vector<int>&,
							 const vector<int>&,vector<int>&) const;

	// setters
	void agregarEje(int,int);
	void buscarEjesEnRestriccion(const vector<double>&,const vector<int>&,double);
	int buscarConCliqueEnRestriccion(const vector<double>&,const vector<int>&,double);


private:

	/* ACLARACIONES:
	 * - los nodos [0..n-1] son originales, los nodos [n..2*n-1] son complemento.
	 * - no agregamos los ejes entre un nodo y su complemento.
	 * - 'graph' es una matriz de adyacencia.
	 */

	int numeroNodos;
	int numeroEjes;
	vector< vector<bool> > graph;
};


#endif // __GRAFO_H__
