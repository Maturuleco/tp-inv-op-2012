#include "covers.hpp"

/** PUBLIC ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Covers::Covers()
{
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


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Covers::agregarRestriccionesTraducidas
	(int r, const vector<double>& vec, const vector<int>& ind, double b)
{
	double acum = 0.0;
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
	}

	rhs[r] = b + acum;
	validas[r] = true;
	agregadas += 1;
} /* toma la r-esima restriccion original, la traduce a desigualdad mochila y la guarda */


// buscar cortes cover ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Covers::buscarCover(int r, const double* x_opt, int tamanho, vector<double>& corte, 
							vector<int>& index, double& b) const
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
		{
			if (agregar[j])
				c += 1;
		}

		// para extender el cover, busco primero el maximo coeficiente de las elegidas
		for (int j = 0; j < vars; j++)
		{
			if (agregar[j] and (restricciones[r][j] > max))
				max = restricciones[r][j];
		}

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

		// ahora armo el plano de corte para el problema
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


bool Covers::resolverMochila
	(int r,const vector<double>& objfunc,vector<bool>& agregar) const
{
	return false;
}
