/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Funciones utiles en cualquier modulo e inclusion de librerias.
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __AUX_H__
#define __AUX_H__


// macros ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define forn(i,n) for(int i = 0; i < (int)(n); i++)
#define forkn(i,k,n) for(int i = (int)(k); i < (int)(n); i++)
#define rforn(i,n) for(int i = (int)(n-1); i >= 0; i--)
#define TOLERANCIA 1E-9

// modulos de c o c++ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <vector>
#include <list>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <fstream>
#include <string.h>
using namespace std;


// funciones ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void errorEnParametros();
void mergeSort(vector<int>&, vector<double>&);


#endif // __AUX_H__
