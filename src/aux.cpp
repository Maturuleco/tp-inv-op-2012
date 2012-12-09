#include "aux.hpp"


// ayuda para usuario ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void errorEnParametros() {
    cout << "\nUSE>\t./resolucion [instancia] [tipo] [cortes]\n\n";
	cout << "  -[instancia]: archivo en formato LP, MPS o SAV\n";
    cout << "  -[tipo]: bb, bc, cb\n";
	cout << "    bb = Branch & Bound\n";
	cout << "    bc = Branch & Cut\n";
	cout << "    cb = Cut & Branch\n";
    cout << "  -[cortes]: cl, co, cc\n";
	cout << "    cl = Cortes clique\n";
	cout << "    co = Cortes cover\n";
	cout << "    cc = Cortes clique + Cortes cover\n";
	cout << "\nEjemplo>\n  ./resolucion knapsack/knapsack.lp bb\n";
	cout << "    -branch and bound sin usar planos de corte-\n";
	cout << "\nEjemplo>\n  ./resolucion knapsack/knapsack.lp bc cc\n";
	cout << "    -branch and cut usando cortes clique y cover-\n\n";
} /* mostrar ayuda de uso para el usuario */


// merge sort ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mergeSort(vector<int>& jotas, vector<double>& base)
{
	int vars = base.size();
	vector<int> jotasBis(vars,0);		// indices de trabajo
	vector<double> trabajo(vars,0.0);	// auxiliar para ordenar

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
} /* devuelve el vector 'base' ordenado de menor a mayor y con indices cambiados */
