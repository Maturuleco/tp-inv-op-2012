#ifndef _RESOLUCION_H_
#define _RESOLUCION_H_

// modulos propios ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "problemaCPLEX.hpp"
#include <string.h>

// ayuda para usuario ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void errorEnParametros() {
    cout << "\nUSE>\tresolucion <*.(lp|mps|sav)> [tipo] [cortes]\n";
    cout << "\t\t[tipo]: bb (Branch&Bound), bc (Branch&Cut), cb (Cut&Branch)\n";
    cout << "\t\t[cortes]: cl (Clique), co (Cover), cc (Clique + Cover)\n";
	cout << "\t\tEjemplo> resolucion knapsack/knapsack.lp bb\n";
	cout << "\t\t-branch and bound sin usar planos de corte-\n";
	cout << "\t\tEjemplo> resolucion knapsack/knapsack.lp bc cc\n";
	cout << "\t\t-branch and cut usando cortes clique y cover-\n\n";
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
