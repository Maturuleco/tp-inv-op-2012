#ifndef _RESOLUCION_H_
#define _RESOLUCION_H_

// modulos propios ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "problemaCPLEX.hpp"
#include <string.h>

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
}


// manejo de excepciones ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool levantaExcepcion(const problemaCPLEX& problema)
{
	if ( problema.hayError() ){
		problema.mostrarError();
		return true;
	}
	return false;
}


#endif // _RESOLUCION_H_
