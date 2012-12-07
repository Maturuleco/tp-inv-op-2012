#include "grafo.hpp"

// constructores ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Grafo::Grafo(): numeroNodos(0), numeroEjes(0), numeroCortes(0) {}


void Grafo::ingresarCantidadDeNodos(int n)
{
	numeroNodos = n;
	vector<bool> p(2*numeroNodos, false);
	graph.resize(2*numeroNodos, p);
} /* inicializacion de la matriz de adyacencia */


// observadores ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Grafo::grafoVacio() const
{
	return (numeroEjes==0);
} /* para saber si se usan cortes clique o no */


bool Grafo::cuantosCortes() const
{
	return numeroCortes;
} /* para saber cuantos cortes clique se agregaron */


// armado de grafo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Grafo::agregarEje(int u,int v)
{
	bool noEstaba = not(graph[u][v] or graph[v][u]);
	graph[u][v] = true;
	graph[v][u] = true;
	if (noEstaba)
		numeroEjes += 1;
} /* agrega un eje al grafo */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Grafo::buscarEjesEnRestriccion
		(const vector<double>& a_r,const vector<int>& indices,double b)
{
	int vars = a_r.size();
	int v_i = 0;
	int v_j = 0;
	double acum = 0.0;
	double aux = 0.0;
	double l_r = 0.0;

	// prendo todas las negativas
	forn(i,vars)
		if (a_r[i] < 0.0)
			acum += a_r[i];

	// agarro de a pares
	forn(i,vars)
	{
		forkn(j,i+1,vars)
		{
			aux = acum;

			if (a_r[i] < 0.0)
				aux -= a_r[i];

			if (a_r[j] < 0.0)
				aux -= a_r[j];

			// todas las combinaciones para prender y apagar
			forn(k,3)
			{
				v_i = k % 2 ; 	/* 0 1 0 1 */
				v_j = k / 2 ; 	/* 0 0 1 1 */

				l_r = aux + v_i*a_r[i] + v_j*a_r[j];

				// si se hace infactible el problema agrego nuevo eje
				if (l_r > b)
				{
					if (v_i == 0)
						v_i = numeroNodos + 1;

					if (v_j == 0)
						v_j = numeroNodos + 1;

					v_i = v_i + indices[i] - 1;
					v_j = v_j + indices[j] - 1;

					agregarEje(v_i, v_j);
				}
			}
		}
	}
} /* para agregar ejes con el primer metodo */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Grafo::buscarConCliqueEnRestriccion
		(const vector<double>& a_r,const vector<int>& indices,double b)
{
	int ejesNuevos = 0;
	int nroCliques = 0;
	int subnodos = a_r.size();	// cantidad de nodos en G(U)
	double acum = 0.0;			// guarda la suma de los coeficientes positivos

	vector<double> w_r(subnodos,0.0);
	vector<int> jotas(subnodos,0);
	vector<bool> complementoEnGdeU(subnodos,true);

	// obtengo el subgrafo G(U)
	forn(i,subnodos)
	{
		w_r[i] = fabs(a_r[i]);
		jotas[i] = i;

		if (a_r[i] > 0.0)
		{
			complementoEnGdeU[i] = false;
			acum += a_r[i];
		}
	}

	// particionar en cliques
	mergeSort(jotas, w_r);
	vector<int> vEnClique(subnodos, subnodos+1);
	nroCliques = particionarEnCliques(w_r, jotas, indices, vEnClique);

	forn(i,subnodos)
		w_r[i] = fabs(a_r[i]);

	// agarro de a pares
	forn(i,subnodos)
	{
		forkn(j,i+1,subnodos)
		{
			// todas las combinaciones para prender y apagar
			forn(k,3)
			{
				int v_i = k % 2 ; 	/* 0 1 0 1 */
				int v_j = k / 2 ; 	/* 0 0 1 1 */
				double l_r = acum;

				bool prendo_i = (v_i == 1) and (not complementoEnGdeU[i]);
				prendo_i = prendo_i or ((v_i == 0) and (complementoEnGdeU[i]));

				bool prendo_j = (v_j == 1) and (not complementoEnGdeU[j]);
				prendo_j = prendo_j or ((v_j == 0) and (complementoEnGdeU[j]));

				// me fijo si es valido el caso a analizar
				if ((vEnClique[i] == vEnClique[j]) and prendo_i and prendo_j)
					continue;

				vector<double> pesoDeCliques(nroCliques,0.0);

				w_r[i] = 0.0;
				w_r[j] = 0.0;

				forn(v, subnodos)
				{
					double maximoActual = pesoDeCliques[ vEnClique[v] ];
					pesoDeCliques[ vEnClique[v] ] = max(maximoActual, w_r[v]);
				}

				w_r[i] = fabs(a_r[i]);
				w_r[j] = fabs(a_r[j]);

				if (prendo_i)
					pesoDeCliques[ vEnClique[i] ] = w_r[i];

				if (prendo_j)
					pesoDeCliques[ vEnClique[j] ] = w_r[j];

				forn(v, nroCliques)
					l_r -= pesoDeCliques[v];

				// si se hace infactible el problema agrego nuevo eje
				if (l_r > b)
				{
					if (v_i == 0)
						v_i = numeroNodos + 1;

					if (v_j == 0)
						v_j = numeroNodos + 1;

					v_i = v_i + indices[i] - 1;
					v_j = v_j + indices[j] - 1;

					agregarEje(v_i, v_j);
				}
			}
		}
	}

	return ejesNuevos;
} /* para agregar ejes con el segundo metodo */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Grafo::particionarEnCliques(const vector<double>& w_r,const vector<int>& jotas,
								const vector<int>& indices, vector<int>& vEnClique) const
{
	// 'jotas' mapea i al indice en la restriccion (i -> a_r)
	// 'indices' mapea i al indice en el grafo (a_r -> nodo real)

	int vars = w_r.size();
	vector<bool> visitados(vars,false);
	int cliqueActual = 0;
	int v = 0;
	int u = 0;
	bool vecinoDeTodos = false;
	bool esVecino = false;

	rforn(i,vars)
	{
		if (visitados[i])
			continue;

		v = jotas[i];
		visitados[i] = true;

		vEnClique[v] = cliqueActual;

		list<int> laClique;
		laClique.push_back(i);	// 'laClique' contiene a 'i' y la voy a extender

		rforn(j,i)
		{
			if (visitados[j])
				continue;

			u = jotas[j];
			vecinoDeTodos = true;

			list<int>::iterator enClique = laClique.begin();

			// agrego al nodo 'u' si es vecino de todos los nodos que estan en 'enClique'
			for (; enClique != laClique.end(); enClique++)
			{
				esVecino = graph[ indices[u] ][ indices[ jotas[*enClique] ] ];

				if (not esVecino)
				{
					vecinoDeTodos = false;
					break;
				}
			}

			if (vecinoDeTodos)
			{
				visitados[j] = true;
				laClique.push_back(j);
				vEnClique[u] = cliqueActual;
			}
		}

		cliqueActual += 1;
	}

	return (cliqueActual+1);
} /* particiona el grafo en cliques enumeradas desde 0 a 'cliqueActual-1' */
