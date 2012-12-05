#include "covers.hpp"

/** PUBLIC ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Covers::Covers()
{
	coversGreedyAgregados = 0;
	coversDinamicosAgregados = 0;
	agregadas = 0;
} /* declaracion */


void Covers::reajustar(int cantRestricciones, int cantVariables)
{
	cant_restricciones = cantRestricciones;
	cant_variables = cantVariables;
	restricciones.resize(cant_restricciones);
	indicadores.resize(cant_restricciones);
	indices.resize(cant_restricciones);
	rhs.resize(cant_restricciones,0.0);
	validas.resize(cant_restricciones,false);
} /* inicializacion una vez declarado */


// observadores ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Covers::estaVacio() const
{
	return ( agregadas == 0 );
} /* para saber si se van a usar o no Cortes Cover, si no se agrego nada no se usa */


int Covers::numeroRestricciones() const
{
	return cant_restricciones;
} /* cantidad de restricciones que tenia el problema original */


int Covers::numeroVariables() const
{
	return cant_variables;
} /* cantidad de variables que tenia el problema original */


bool Covers::puedoBuscarEnRestriccion(int r) const
{
	return validas[r];
} /* para saber si tengo la traduccion en desigualdad mochila de restriccion r-esima */


int Covers::cuantosGreedy() const
{
	return coversGreedyAgregados;
} /* para saber cuantos cortes agrego con el algoritmo goloso */


int Covers::cuantosDinamicos() const
{
	return coversDinamicosAgregados;
} /* para saber cuantos cortes agrego con el algoritmo exacto */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Covers::agregarRestriccionesTraducidas
	(int r, const vector<double>& vec, const vector<int>& ind, double b)
{
	if (vec.size() == 0)
		return;

	double acum = 0.0;
	double minCoef = fabs(vec[0]);
	int vars = ind.size();

	restricciones[r].resize(vars, 0.0);
	indicadores[r].resize(vars, false);
	indices[r].resize(vars,0);

	for (int j = 0; j < vars; j++)
	{
		double a_rj = vec[j];
		indices[r][j] = ind[j];
		restricciones[r][j] = a_rj;

		if (a_rj < 0)
		{
			acum += fabs(a_rj);
			indicadores[r][j] = true;
			restricciones[r][j] = fabs(a_rj);
		}

		if (minCoef > fabs(a_rj))
			minCoef = fabs(a_rj);
	}

	rhs[r] = b + acum;

	if (minCoef < 1)
	{
		for (int j = 0; j < vars; j++)
		{
			restricciones[r][j] /= minCoef;
		}
		rhs[r] /= minCoef;
	}

	validas[r] = true;
	agregadas += 1;
} /* toma la r-esima restriccion original, la traduce a desigualdad mochila y la guarda */


// buscar cortes cover ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Covers::buscarCover(int r, const double* x_opt, int tamanho, vector<double>& corte, 
							vector<int>& index, double& b)
{
	int vars = restricciones[r].size();
	vector<bool> agregar(vars,false);
	vector<double> objfunc(vars,0.0);

	// obtengo los coeficientes de la funcion objetivo del problema de mochila
	for (int j = 0; j < vars; j++)
	{
		double x_j = x_opt[ indices[r][j] ];
		objfunc[j] = 1.0 - x_j;

		if (indicadores[r][j])
			objfunc[j] = x_j;
	}

	if (resolverMochila(r, objfunc, agregar))
	{
		int c = 0;
		int negativos = 0;
		double max = 0.0;

		// calculo cuantas variables fueron elegidas para el cover
		for (int j = 0; j < vars; j++)
			if (agregar[j])
				c += 1;

		// para extender el cover, busco primero el maximo coeficiente de las elegidas
		for (int j = 0; j < vars; j++)
			if (agregar[j] and (restricciones[r][j] > max))
				max = restricciones[r][j];

		// para extender cover, agrego variables no elegidas de mayor coeficiente
		for (int j = 0; j < vars; j++)
		{
			if (not(agregar[j]) and (restricciones[r][j] >= max))
			{
				c += 1;
				agregar[j] = true;
			}
		}

		corte.resize(c, 0.0);
		index.resize(c, 0);

		// ahora armo el plano de corte para el problema, cambiando variables
		for (int h = 0, j = 0; j < vars; j++)
		{
			if (agregar[j])
			{
				index[h] = indices[r][j];
				corte[h] = 1.0;

				if (indicadores[r][j])
				{
					corte[h] = -1.0;
					negativos += 1;
				}

				h += 1;
			}
		}

		// asigno el rhs del plano de corte
		b = c - negativos - 1.0;
	}

} /* buscar un corte cover a partir del optimo de la relajacion */



/** PRIVATE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Covers::resolverMochila
	(int r,const vector<double>& objfunc,vector<bool>& agregar)
{
	// intento algoritmo goloso
	if (resolverMochilaGreedy(r,objfunc,agregar))
	{
		coversGreedyAgregados += 1;
		return true;
	}

	// intento algoritmo programacion dinamica
	if (resolverMochilaDinamica(r,objfunc,agregar))
	{
		coversDinamicosAgregados += 1;
		return true;
	}

	return false;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Covers::resolverMochilaGreedy
				(int r,const vector<double>& objfunc,vector<bool>& agregar) const
{
	int vars = objfunc.size();
	double acum = 0.0;
	double limite = 0.0;

	vector<int> jotas(vars,0);			// indices de base
	vector<double> base(vars,0.0);		// beneficios (1-x_j)/a_rj

	vector<int> jotasBis(vars,0);		// indices de trabajo
	vector<double> trabajo(vars,0.0);	// auxiliar para ordenar

	// lleno vector de beneficios
	for (int j = 0; j < vars; j++)
	{
		base[j] = objfunc[j] / floor(restricciones[r][j]);
		jotas[j] = j;
	}

	// merge sort de beneficios
	for (int width = 1; width < vars; width *= 2)
	{
		for (int k = 0; k < vars; k += 2*width)
		{
			int beg = k;
			int mid = min(k+width,vars);
			int end = min(k+2*width,vars);

			int left = beg;
			int right = mid;

			for (int e = beg; e < end; e++)
			{
				if ((left < mid) and (right >= end || base[left] <= base[right]))
				{
					trabajo[e] = base[left];
					jotasBis[e] = jotas[left];
					left += 1;
				}
				else
				{
					trabajo[e] = base[right];
					jotasBis[e] = jotas[right];
					right += 1;
				}
			}
		}

		base = trabajo;
		jotas = jotasBis;
	}

	// busco superar cota segun el orden de beneficios
	for (int j = 0; limite < ceil(rhs[r]) + 1.0 ; j++)
	{
		if (j == vars)
			return false;

		int x_j = jotas[j];
		acum += objfunc[x_j];
		limite += floor( restricciones[r][x_j] );
		agregar[x_j] = true;
	}

	return (acum < 1);
} /* si encuentra, obtiene un corte cover golosamente */


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Covers::resolverMochilaDinamica
				(int r,const vector<double>& objfunc,vector<bool>& agregar) const
{
	int vars = objfunc.size();
	int minimo = (int)ceil(rhs[r]) + 1;
	int maximo = 0;
	double acum = 0.0;

	// voy a suponer que la mochila agrega primero todas las variables
	for (int j = 0; j < vars; j++)
	{
		maximo += (int)floor( restricciones[r][j] );
		acum += objfunc[j];
	}

	// me fijo si escala el algoritmo con limite 1000
	if ((maximo-minimo > 1000) or (minimo<0))
	{
		return false;
	}

	// preparo tablita de programacion dinamica
	int pesoExtra = maximo-minimo;
	vector<double> p(pesoExtra+1, acum);
	vector< bool > q(pesoExtra+1, false);
	vector< vector<double> > tabla(vars+1, p);
	vector< vector< bool > > apago(vars+1, q);

	// completo tabla de programacion dinamica
	/*
	 * tabla[j][w] =	el minimo benefico obtenido usando hasta j-1 variables
	 *					y sacando hasta w unidades de peso
	 */
	for (int j = 1 ; j <= vars; j++)
	{
		for (int w = 0; w <= pesoExtra; w++)	/* w indica cuanto peso puedo sacar */
		{
			int w_j = (int)floor(restricciones[r][j-1]);
			if (w < w_j)
			{
				tabla[j][w] = tabla[j-1][w];
			}
			else
			{
				double valor1 = tabla[j-1][w];
				double valor2 = tabla[j-1][w - w_j] - objfunc[j-1];

				if (valor2 > valor1)
				{
					apago[j][w] = false;
					tabla[j][w] = valor1;
				}
				else
				{
					apago[j][w] = true;
					tabla[j][w] = valor2;
				}
			}
		}
	}

	// me fijo que variables estoy usando en el cover
	for (int j = vars, w = pesoExtra; j >= 1; j--)
	{
		int w_j = (int)floor(restricciones[r][j-1]);
		if (apago[j][w])
		{
			agregar[j-1] = false;
			w -= w_j;
		}
		else
		{
			agregar[j-1] = true;
		}
	}

	// me fijo si tengo un cover violado
	acum = 0.0;
	for (int j = 0; j < vars; j++)
		if (agregar[j])
			acum += objfunc[j];

	return (acum < 1);
} /* si encuentra, obtiene un corte cover de forma exacta */
