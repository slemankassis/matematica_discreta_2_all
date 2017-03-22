/*
  Materia: Matematica Discreta 2
  Profesor: Penazzi
  Año: 2016
  Integrantes:
  Sleman Kassis, slemankassis@gmail.com
  Jose Rodriguez, jl.rodriguezgm@gmail.com
*/

#ifndef APIFILES_CTHULHU_H_
#define APIFILES_CTHULHU_H_

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint32_t u32;
typedef struct NimheSt *NimheP;
typedef struct VerticeStt *VerticeSt;

////////////////////////////////////////////////////////////////////////////////
// Funciones de crear y destruir grafo.

NimheP NuevoNimhe();
/*
La funcion aloca memoria, inicializa lo que haya que inicializar de una 
estructura NimheSt y devuelve un puntero a esta. Ademas lee un grafo desde
standard input en el formato especificado al principio de este documento y llena
esa estructura NimheSt con esos datos.
Ademas de cargar el grafo, asigna el “color” 0 a cada vertice para indicar que
estan todos no coloreados.
En caso de error, la funcion devolvera un puntero a NULL. (errores posibles
pueden ser falla en alocar memoria, pero tambien que el formato de entrada no
sea valido, por ejemplo, que la primera linea sin comentarios no empieze con p o
que n.
*/

int DestruirNimhe(NimheP G);
/*
Destruye G y libera la memoria alocada. Retorna 1 si todo anduvo bien y 0 si no.
*/


////////////////////////////////////////////////////////////////////////////////
// Funciones de vertices.

u32 ColorDelVertice(VerticeSt x);
/*
Devuelve el color con el que esta coloreado el vertice x. Si el vertice no esta
coloreado, devuelve 0.
*/

u32 GradoDelVertice(VerticeSt x);
/*
Devuelve el grado del vertice x.
*/

u32 NombreDelVertice(VerticeSt x);
/*
Devuelve el nombre real del vertice x.
*/

void ImprimirVecinosDelVertice(VerticeSt x);
/*
Imprime en standard output una lista de los vecinos de x. La lista debe estar
formada por los nombres reales de los vertices, separados por comas, con un
punto al final de la lista.
*/


////////////////////////////////////////////////////////////////////////////////
// Funciones de informacion de grafo.

u32 NumeroDeVertices(NimheP G);
/*
Devuelve el n´umero de vertices del grafo G.
*/

u32 NumeroDeLados(NimheP G);
/*
Devuelve el numero de lados del grafo G.
*/

u32 NumeroVerticesDeColor(NimheP G, u32 i);
/*
Retorna el numero de vertices de color i. (si i = 0 devuelve el numero de
vertices no coloreados).
*/

u32 ImprimirVerticesDeColor(NimheP G, u32 i);
/*
Imprime una linea que dice “Vertices de Color i:” y a continuacion una lista de
los vertices que tienen el color i, separados por comas y con un punto luego del
ultimo color. (si i = 0 esta lista sera la lista de vertices no coloreados).
Los vertices no tienen porque estar ordenados.
Si no hay vertices de color i debe imprimir “No hay vertices de color i”
Retorna el numero de vertices de color i.
*/

u32 CantidadDeColores(NimheP G);
/*
Devuelve la cantidad de colores usados en el coloreo de G.
*/

VerticeSt IesimoVerticeEnElOrden(NimheP G, u32 i);
/*
Devuelve el vertice numero i en el orden guardado en ese momento en G. (el
indice 0 indica el primer vertice, el iındice 1 el segundo, etc).
*/

VerticeSt IesimoVecino(NimheP G, VerticeSt x, u32 i);
/*
Devuelve el vecino numero i de x en el orden en que lo tengan guardado uds. en
G. (el orden es irrelevante, lo importante es que de esta forma podemos pedir
externamente la lista de vecinos) (el indice 0 indica el primer vertice, el
indice 1 el segundo, etc).
*/


////////////////////////////////////////////////////////////////////////////////
// Funciones de coloreo.

int Chidos(NimheP G);
/*
Devuelve 1 si χ(G) = 2, 0 si no.
*/

u32 Greedy(NimheP G);
/*
Corre greedy en G con el orden interno que debe estar guardado de alguna forma
dentro de G.
Devuelve el numero de colores que se obtiene.
*/


////////////////////////////////////////////////////////////////////////////////
// Funciones de ordenacion.

void OrdenNatural(NimheP G);
/*
Ordena los vertices en orden creciente de sus “nombres” reales. (recordar que el
nombre real de un vertice es un u32).
*/

void OrdenWelshPowell(NimheP G);
/*
Ordena los vertices de G de acuerdo con el orden Welsh-Powell, es decir, con los
grados en orden no creciente.
*/

void ReordenAleatorioRestringido(NimheP G);
/*
Si G esta coloreado con r colores y W1 son los vertices coloreados con 1, W2 los
coloreados con 2, etc, entonces esta funcion ordena los vertices poniendo
primero los vertices de Wj1 (en algun orden) luego los de Wj2 (en algun orden),
etc, donde j1, j2, ... son aleatorios (pero distintos).
*/

void GrandeChico(NimheP G);
/*
Si G esta coloreado con r colores y W1 son los vertices coloreados con 1, W2 los
coloreados con 2, etc, entonces esta funci´on ordena los vertices poniendo
primero los vertices de Wj1 (en algun orden) luego los de Wj2 (en algun orden),
etc, donde j1, j2, ... son tales que |Wj1| >= |Wj2| >= ... >= |Wjr|.
*/

void ChicoGrande(NimheP G);
/*
Idem que el anterior excepto que ahora el orden es tal que |Wj1| <= |Wj2| <= ...
<= |Wjr|.
*/

void Revierte(NimheP G);
/*
Si G esta coloreado con r colores y W1 son los vertices coloreados con 1, W2 los
coloreados con 2, etc, entonces esta funcion ordena los vertices poniendo
primero los vertices de Wr (en algun orden) luego los de Wr−1 (en algun orden),
etc.
*/

void OrdenEspecifico(NimheP G, u32* x);
/*
La imagen de x deben ser todos los numeros entre 0 y n − 1, sin repetir. La
funcion primero chequea que x cumpla esa condicion. Si no lo hace, la funcion
deja el orden de G como estaba.
Si x cumple la condici´on, entonces la funcion ordena los vertices con
OrdenNatural(), luego lee el orden dado en la string x y los ordena de acuerdo a
lo que lee. Es decir, si luego de OrdenNatural los vertices quedaron ordenados
como, por ejemplo, V[0],V[1],...,V[n-1], luego de OrdenEspecifico el orden debe
ser V[x[0]],V[x[1]],...,V[x[n-1]].
*/


#endif  // APIFILES_CTHULHU_H_
