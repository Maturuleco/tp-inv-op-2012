/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Resolver para problemas de programacion lineal con todas variables binarias.
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __RESOLUCION_H__
#define __RESOLUCION_H__


#include "problemaCPLEX.hpp"


// manejo de excepciones ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool levantaExcepcion(const problemaCPLEX& problema)
{
	if ( problema.hayError() ){
		problema.mostrarError();
		return true;
	}
	return false;
} /* para saber si hay que mostrar un error de CPLEX */


#endif // __RESOLUCION_H__
