#include "grafo.hpp"

/** PUBLIC ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// constructores ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Grafo::Grafo(): numeroNodos(0), numeroEjes(0), numeroCortes(0) {}


void Grafo::ingresarCantidadDeNodos(int n)
{
	numeroNodos = n;
	graph.resize(2*numeroNodos);
} /* inicializacion de la matriz de adyacencia */


// observadores ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Grafo::grafoVacio() const
{
	return (numeroNodos*numeroEjes == 0);
} /* para saber si se usan cortes clique o no */


int Grafo::cuantosCortes() const
{
	return numeroCortes;
} /* para saber cuantos cortes clique se agregaron */


int Grafo::cuantosEjes() const
{
	return numeroEjes;
} /* para saber cuantos ejes tiene el grafo */


int Grafo::gradoDeNodo(int nodo) const
{
	return graph[nodo].size();
} /* para saber cuantos vecinos tiene un nodo */


const list<int>& Grafo::vecinosDeNodo(int nodo) const
{
	return graph[nodo];
} /* para saber quienes son los vecinos de un nodo */


void Grafo::mostrar() const
{
	FILE* output = fopen("graph.txt","w");
	fprintf(output,"%d nodos %d ejes\n", numeroNodos, numeroEjes);
	forn(i,2*numeroNodos){
		list<int>::const_iterator nb = vecinosDeNodo(i).begin(); 
		list<int>::const_iterator end = vecinosDeNodo(i).end();
		for( ; nb != end; nb++){
			fprintf(output,"nodo %i -> %i ",i,*nb);
		}
		fprintf(output,"\n");
	}
	fclose(output);
} /* para mostrar el grafo */


bool Grafo::sonVecinos(int u,int v,bool uComplemento, bool vComplemento) const
{
	u += (uComplemento)? numeroNodos : 0;
	v += (vComplemento)? numeroNodos : 0;

	int elegido = v;
	list<int>::const_iterator nb = vecinosDeNodo(u).begin();
	list<int>::const_iterator end = vecinosDeNodo(u).end();

	if (gradoDeNodo(u) > gradoDeNodo(v))
	{
		elegido = u;
		nb = vecinosDeNodo(v).begin();
		end = vecinosDeNodo(v).end();
	}

	for(; nb != end; nb++)
		if (elegido == *nb)
			return true;

	return false;		
} /* para saber si los nodos 'u' y 'v' son vecinos */


// armado de grafo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Grafo::agregarEje(int u,int v, bool uComplemento, bool vComplemento)
{
	u += (uComplemento)? numeroNodos : 0;
	v += (vComplemento)? numeroNodos : 0;

	if (not sonVecinos(u,v))
	{
		graph[v].push_back(u);
		graph[u].push_back(v);
		numeroEjes += 1;
	}	
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
			forn(k,4)
			{
				v_i = k % 2 ; 	/* 0 1 0 1 */
				v_j = k / 2 ; 	/* 0 0 1 1 */

				l_r = aux + v_i*a_r[i] + v_j*a_r[j];

				// si se hace infactible el problema agrego nuevo eje
				if (l_r > b)
					agregarEje(indices[i], indices[j], v_i==0, v_j==0);
			}
		}
	}

} /* para agregar ejes con el primer metodo */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Grafo::buscarConCliqueEnRestriccion
		(const vector<double>& a_r,const vector<int>& indices,double b)
{
	int nroCliques = 0;
	int subnodos = a_r.size();	// cantidad de nodos en G(U)
	double acum = 0.0;			// guarda la suma de los coeficientes positivos

	vector<double> w_r(subnodos,0.0);
	vector<int> jotas(subnodos,0);
	vector<bool> complementoEnGdeU(subnodos,false);

	// obtengo el subgrafo G(U)
	forn(i,subnodos)
	{
		w_r[i] = fabs(a_r[i]);
		jotas[i] = i;

		if (a_r[i] > 0.0)
		{
			complementoEnGdeU[i] = true;
			acum += a_r[i];
		}
	}

	// particionar en cliques
	mergeSort(jotas, w_r);
	vector<int> vEnClique(subnodos, subnodos+1);
	nroCliques = particionarEnCliques(jotas, indices, complementoEnGdeU, vEnClique);

	forn(i,subnodos)
		w_r[i] = fabs(a_r[i]);

	// agarro de a pares
	forn(i,subnodos)
	{
		forkn(j,i+1,subnodos)
		{
			// todas las combinaciones para prender y apagar
			forn(k,4)
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
					agregarEje(indices[i], indices[j], v_i==0, v_j==0);
			}
		}
	}

} /* para agregar ejes con el segundo metodo */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int Grafo::particionarEnCliques(const vector<int>& jotas,const vector<int>& indices,
								const vector<bool>& cEnGdeU,vector<int>& vEnClique) const
{
	// 'jotas' mapea i al indice en la restriccion (i -> a_r)
	// 'indices' mapea i al indice en el grafo (a_r -> nodo real)

	int v = 0;
	int v_real = 0;
	int u = 0;
	int u_real = 0;
	int cliqueActual = 0;
	int vars = jotas.size();
	vector<bool> visitados(vars,false);
	bool vecinoDeTodos = false;
	bool esVecino = false;

	// inicio cada clique
	rforn(i,vars)
	{
		if (visitados[i])
			continue;

		list<int> laClique;

		v = jotas[i];
		v_real = (cEnGdeU[v])? (indices[v] + numeroNodos) : (indices[v]);
		visitados[i] = true;
		laClique.push_back(v_real);
		vEnClique[v] = cliqueActual;

		// extiendo clique
		rforn(j,i)
		{
			if (visitados[j])
				continue;

			u = jotas[j];
			u_real = (cEnGdeU[u])? (indices[u] + numeroNodos) : (indices[u]);

			if (nodoExtiendeClique(u_real,laClique))
			{
				visitados[j] = true;
				laClique.push_back(u_real);
				vEnClique[u] = cliqueActual;
			}
		}

		cliqueActual += 1;
	}

	return (cliqueActual+1);
} /* particiona el grafo en cliques enumeradas desde 0 a 'cliqueActual-1' */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Grafo::buscarClique(int nodo,const double* x_opt,int vars,
							vector<double>& corte,vector<int>& indices,double& rhs)
{
	list<int> clique;
	clique.push_back(nodo);
	int cVecinos = gradoDeNodo(nodo);

	// si no tiene vecinos no hay clique para buscar
	if (cVecinos == 0)
		return;

	vector<double> pesoVecinos(cVecinos, 0.0);
	vector<int> indVecinos(cVecinos, 0);
	vector<int> vecinos(cVecinos, 0);
	list<int>::const_iterator vecino = vecinosDeNodo(nodo).begin();

	// ordeno los vecinos por su valor en el optimo
	for(int i = 0; i < cVecinos; vecino++, i++)
	{
		vecinos[i] = *vecino;
		indVecinos[i] = i;
		if (*vecino < numeroNodos)
			pesoVecinos[i] = x_opt[*vecino];
		else
			pesoVecinos[i] = 1.0 - x_opt[*vecino- numeroNodos];
	}

	//// HEURISTICA PROMETEDORA: si el nodo y sus vecinos no suman 1, no busco corte
	double prometedora = x_opt[nodo];
	forn(i,cVecinos)
		prometedora += fabs(pesoVecinos[i]);
	if (prometedora <= 1.0)
		return;
	//// FIN HEURISTICA PROMETEDORA

	mergeSort(indVecinos, pesoVecinos);

	// armo la clique maximal agregando nodos vecinos a todos los de la clique
	rforn(i,cVecinos)
	{
		int neighb = vecinos[ indVecinos[i] ];
		if (nodoExtiendeClique(neighb,clique))
			clique.push_back(neighb);
	}

	// me fijo si es un corte
	double acum = 0.0;
	int negativos = 0;
	vector<int> indicesBis(clique.size(), 0);
	vector<double> corteBis(clique.size(), 0.0);
	list<int>::const_iterator enClique = clique.begin();
	for (int i = 0; enClique != clique.end(); enClique++, i++)
	{
		if (*enClique < numeroNodos)
		{
			acum += x_opt[*enClique];
			corteBis[i] = 1.0;
			indicesBis[i] = *enClique;
		}
		else
		{
			acum += 1.0 - x_opt[*enClique - numeroNodos];
			corteBis[i] = -1.0;
			indicesBis[i] = *enClique - numeroNodos;
			negativos += 1;
		}
	}
	
	// si es un corte le aviso a la funcion que me llamo
	if ((acum > 1.0) and (clique.size() > 1))
	{
		rhs = 1.0 - negativos;
		corte = corteBis;
		indices = indicesBis;
		numeroCortes += 1;
	}

} /* busca una clique maximal que contenga a 'nodo' */



/** PRIVATE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Grafo::nodoExtiendeClique(int nodo, const list<int>& clique) const
{
	list<int>::const_iterator nb = clique.begin();
	list<int>::const_iterator fin = clique.end();

	for(; nb != fin; nb++)
		if (not sonVecinos(nodo,*nb))
			return false;

	return true;
}
