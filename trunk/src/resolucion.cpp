#include "resolucion.hpp"

int main(int argc, char **argv) {

//// parametros del usuario
    if (argc != 4 && argc != 3) {
        errorEnParametros();
		return 1;
    }

    char* archivoLP = argv[1];
	bool usoCliques = false;
	bool usoCovers = false;
	bool cutandbranch = false;
	bool branchandcut = false;

	for (int e = 0; e < argc; e++)
	{
		if (strcmp( argv[e], "bc" ) == 0) {
			branchandcut = true;
		} else if (strcmp( argv[e], "cb") == 0) {
			cutandbranch = true;
		} else if (strcmp( argv[e], "cl") == 0) {
			usoCliques = true;
		} else if (strcmp( argv[e], "co") == 0) {
			usoCovers = true;
		} else if (strcmp( argv[e], "cc") == 0) {
			usoCliques = true;
			usoCovers = true;
		}
	}


//// inicio y configuro el cplex
	problemaCPLEX problema;
	if ( levantaExcepcion(problema) ) { return 1; }

	problema.deshabilitarParametros();
	if ( levantaExcepcion(problema) ) { return 1; }

	problema.leerLP( archivoLP );
	if ( levantaExcepcion(problema) ) { return 1; }

	if (problema.numeroVariables() * problema.numeroRestricciones() == 0)
	{
		fprintf(stderr,"Error inesperado. No hay variables o restricciones.\n");
		return 1;
	}

	problema.setearTiempoMaximo(3600.0); 				/* 3600.0 segs = 1 hr. */
	if ( levantaExcepcion(problema) ) { return 1; }

	problema.elegirEstrategiaDeSeleccionDeNodo(); 		/* se usa 'best-bound' */
	if ( levantaExcepcion(problema) ) { return 1; }

	problema.elegirEstrategiaDeSeleccionDeVariable();	/* se usa 'min-infeas' */
	if ( levantaExcepcion(problema) ) { return 1; }

	problema.configuracion(branchandcut, cutandbranch, usoCliques, usoCovers);
	if ( levantaExcepcion(problema) ) { return 1; }


//// resuelvo y muestro resultados
	problema.resolverMIP();
	if ( levantaExcepcion(problema) ) { return 1; }
	
	problema.mostrarSolucion();
	if ( levantaExcepcion(problema) ) { return 1; }

	return 0;
}
