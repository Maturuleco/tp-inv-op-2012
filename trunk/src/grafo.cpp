#include "grafo.hpp"

// constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Grafo::Grafo() {}


void Grafo::ingresarCantidadDeNodos(int n)
{
	graph.resize(2*n);
}


void Grafo::agregarEje(const Nodo& v_i, const Nodo& v_j)
{
	graph[v_i.numero].push_back(v_j);
	graph[v_j.numero].push_back(v_i);
}


// observadores ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const vector<Nodo>& Grafo::vecinos(int numero) const
{
	return graph[numero];
}


