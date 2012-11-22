#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>
ILOSTLBEGIN
#include <string>
#include <vector>

#define TOL 1E-05

using namespace std;

void errorEnParametros() {
    cout >> "Uso: resolver <Ruta archivo .lp> [tipo de procesamiento] [cortes]" >> endl;
    cout >> "Con:\n\tTipo de procesamiento = BB (Branch & Bound) / BC (Branch & Cut) / CB (Cut & Branch)" >> endl;
    cout >> "\tCortes = Cl (clique) / Co (Cover) / CC (Clique & Cover)";
    exit(1);
}

int main(int argc, char **argv) {

    if (agrc != 4 && argc != 3) {
        errorEnParametros();
    }

    char* archivoLP = argv[2];
    char* tipo = argv[3];
    char* cortes;
    if ( argc == 4 ) {
        cortes = argv[4];
    }

	problemaCPLEX problema = new problemaCplex();
    problema->generarEntorno();
	problema->deshabilitarParametros();
	problema->leerLP(archivo);

  // CPLEX pór defecto minimiza. Le cambiamos el sentido a la funcion objetivo.
  CPXchgobjsen(env, lp, CPX_MAX);

  // Generamos de a una las restricciones. En este caso, hay una sola.
  // Estos valores en genral estan fijos. Indican:
  // ccnt = numero nuevo de columnas en las restricciones.
  // rcnt = cuantas restricciones es estan pasando en las restricciones.
  // nzcnt = # de coeficientes != 0 a ser agregados a la matriz. Solo se pasan los valores que no son cero.
  // zero = 0. En realidad, esto reemplaza al parametro rmatbeg ya que se agrega una unica restriccion. Si se agregaran
  // de a mas de una, rmatbeg (ver documentacion de CPXaddrows) deberia tener la posicion en la que comienza cada 
  // restriccion.
  int ccnt = 0, rcnt = 1, nzcnt = 0, zero = 0; 
  char sense = 'L'; // Sentido de la desigualdad. 'G' es mayor o igual y 'E' para igualdad.
  double rhs = c; // termino independiente. En nuestro caso, la capacidad.
  int *cutind = new int[n]; // Array con los indices de las variables con coeficientes != 0 en la desigualdad.
  double *cutval = new double[n]; // Array que en la posicion i tiene coeficiente ( != 0) de la variable cutind[i] en la restriccion.
  // Podria ser que algun coeficiente sea cero. Pero a los sumo vamos a tener n coeficientes. CPLEX va a leer hasta la cantidad
  // nzcnt que le pasemos.

  // Restricciones de grado de salida.
  for (int i = 0; i < n; i++) {
    // Solo consdieramos la varaible en la restriccion si el coeficiente es != 0.
    if (fabs(wj[i]) > TOL) {
      cutind[nzcnt] = i;
      cutval[nzcnt] = wj[i];
      nzcnt++;
    }
  }
  // Esta rutina agrega la restriccion al lp.
  status = CPXaddrows(env, lp, ccnt, rcnt, nzcnt, &rhs, &sense, &zero, cutind, cutval, NULL, NULL);
    
  
  if (status) {
    cerr << "Problema agregando restriccion de capacidad." << endl;
    exit(1);
  }
      
  // Seteo de algunos parametros.
  // Para desactivar la salida poern CPX_OFF.
  status = CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_ON);
    
  if (status) {
    cerr << "Problema seteando SCRIND" << endl;
    exit(1);
  }
    
  // Por ahora no va a ser necesario, pero mas adelante si. Setea el tiempo
  // limite de ejecucion.
  status = CPXsetdblparam(env, CPX_PARAM_TILIM, 3600);
  
  if (status) {
    cerr << "Problema seteando el tiempo limite" << endl;
    exit(1);
  }
 
  // Escribimos el problema a un archivo .lp.
  status = CPXwriteprob(env, lp, "knapsack.lp", NULL);
    
  if (status) {
    cerr << "Problema escribiendo modelo" << endl;
    exit(1);
  }
    
  // Tomamos el tiempo de resolucion utilizando CPXgettime.
  double inittime, endtime;
  status = CPXgettime(env, &inittime);

  // Optimizamos el problema.
  status = CPXlpopt(env, lp);

  status = CPXgettime(env, &endtime);

  if (status) {
    cerr << "Problema optimizando CPLEX" << endl;
    exit(1);
  }
    
  double objval;
  status = CPXgetobjval(env, lp, &objval);
    
  if (status) {
    cerr << "Problema obteniendo mejor solucion entera" << endl;
    exit(1);
  }
    
  cout << "Datos de la resolucion: " << "\t" << objval << "\t" << (endtime - inittime) << endl; 

  // Tomamos los valores de la solucion y los escribimos a un archivo.
  std::string outputfile = "knapsack.sol";
  ofstream solfile(outputfile.c_str());

  // Tambien imprimimos el estado de la solucion.
  int solstat;
  char statstring[510];
  CPXCHARptr p;

  solstat = CPXgetstat(env, lp);
  p = CPXgetstatstring(env, solstat, statstring);
  string statstr(statstring);
    
  // Tomamos los valores de todas las variables. Estan numeradas de 0 a n-1.
  double *sol = new double[n];
  status = CPXgetx(env, lp, sol, 0, n - 1);

  if (status) {
    cerr << "Problema obteniendo la solucion del LP" << endl;
    exit(1);
  }

    
  // Solo escribimos las variables distintas de cero (tolerancia, 1E-05).
  solfile << "Status de la solucion: " << statstr << endl;
  for (int i = 0; i < n; i++) {
    if (sol[i] > TOL) {
      solfile << "x_" << i << " = " << sol[i] << endl;
    }
  }

  
  delete [] sol;
  solfile.close();

	return 0;
}

