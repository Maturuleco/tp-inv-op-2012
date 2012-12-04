#ifndef _GRAFO_H_
#define _GRAFO_H_

// modulos de c o c++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <vector>
#include <iostream>
#include <math.h>
using namespace std;


// macros ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define forn(i,n) for(int i = 0; i < (int)(n); i++)


// structs ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct Nodo {
	int numero;		// [0..n-1] originales [n..2n-1] complemento
	double peso;
};


// definicion ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class Grafo {

public:

	Grafo();
	void ingresarCantidadDeNodos(int);

	void agregarEje(const Nodo&, const Nodo&);

	const vector<Nodo>& vecinos(int numero) const;
	list< list<Nodo> > particionarEnCliques(const list<int>&) const;

private:

	vector< vector<Nodo> > graph;

};


#endif // _GRAFO_H_
