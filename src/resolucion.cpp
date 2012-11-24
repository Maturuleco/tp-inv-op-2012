#include "resolucion.hpp"

// ayuda para usuario ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void errorEnParametros() {
    cout << "USE>\tresolver <*.(lp|mps|sav)> [tipo] [cortes]" << endl;
    cout << "\t\ttipo: BB (Branch&Bound) - BC (Branch&Cut) - CB (Cut&Branch)" << endl;
    cout << "\t\tcortes: Cl (clique) - Co (Cover) - CC (Clique & Cover)" << endl;
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


// resolver ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(int argc, char **argv) {

//// parametros del usuario
    if (argc != 4 && argc != 3) {
        errorEnParametros();
		return 1;
    }

    char* archivoLP = argv[1];
    char* tipo = argv[2];
    char* cortes;
    if ( argc == 4 ) {
        cortes = argv[3];
    }


//// preparo el cplex
	problemaCPLEX problema;
	if ( levantaExcepcion(problema) ) { return 1; }

	problema.deshabilitarParametros();
	if ( levantaExcepcion(problema) ) { return 1; }

	problema.leerLP(archivoLP);
	if ( levantaExcepcion(problema) ) { return 1; }


//// resuelvo y muestro resultados
	problema.resolverMIP();
	if ( levantaExcepcion(problema) ) { return 1; }
	
	problema.mostrarSolucion();
	if ( levantaExcepcion(problema) ) { return 1; }

	return 0;
}
