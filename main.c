#include <stdio.h>
#include <stdlib.h>
#include "Cthulhu.h"


////////////////////////////////////////////////////////////////////////////////
// Funciones AUXILIARES de main.

void ReordenMain(u32 *array, u32 n) {
    if (n > 1) {
        u32 i;
        for (i = 0; i < n - 1; i++) {
            u32 j = i + rand() / (RAND_MAX / (n - i) + 1);
            u32 t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Main.

/*

1.

Carga el grafo. Si el formato de entrada esta mal imprime una linea que dice
“Error en formato de entrada” y para. (por ejemplo, si hay una linea que empieza
con “d”, o si la primera linea que no empieza con “c” no empieza con “p”, o si
el numero de vertices que se extrae de los lados no es n, etc).

2.

Si X(G) = 2 imprime en standard output “Grafo Bipartito” y para.

3.

Si X(G) > 2 imprime en una linea “Grafo No Bipartito” y luego crea 10
ordenes aleatorios mas Welsh-Powell y corre Greedy con cada uno de esos ordenes
y dice cuantos colores obtiene en cada caso, en 11 lineas separadas.

En el caso de las corridas aleatorias cada linea sera de la forma
“coloreo aleatorio numero k: r colores”
donde r sera el numero de colores que obtuvieron y k el numero de corrida
aleatoria.

En el caso del orden WelshPowell la linea sera
“ coloreo con Greedy en WelshPowell:r colores”

4.

Si esta en este paso es porque el grafo no es bipartito, por lo tanto si alguno
de esos coloreos es con tres colores, se sabe que X(G) = 3 asi que se
imprime una linea que dice “X(G) = 3” y para.

5.

Si no se obtuvieron 3 colores con ninguno de los coloreos, imprime
“====Comenzando Greedy Iterado 1001 veces====”

Luego se toma el mejor coloreo de los primeros 11 que se hicieron y se corre
Greedy 1000 veces cambiando el orden de los colores, siguiendo el siguiente
patron:
  50% ChicoGrande
  12,5% GrandeChico
  31,25% Revierte
  6,25% ReordenAleatorioRestringido
(nota: esos porcentajes son 8/16, 2/16, 5/16 y 1/16 respectivamente).

Luego de estas 1000 iteraciones, se hace una iteracion final con Revierte, e
imprime:
Mejor coloreo con Greedy iterado 1001 veces: k colores
(a CG,b GC, c R, d RAR)

donde k es el menor numero de colores que hayan obtenido en las 1001
iteraciones, y a,b,c,d son la cantidad de veces que corrieron Chico Grande,
GrandeChico, Revierte y ReordenAleatorioRestringido, respectivamente.

Estas condiciones no significan que exactamente un 12,5% van a usar GrandeChico,
exactamente un 50% van a usar ChicoGrande, etc, sino que en cada iteracion,
cuando el algoritmo va a decidir cual orden usara, elegira GrandeChico con una
probabilidad del 12,5%, ChicoGrande con una probabilidad del 50%, etc.
*/

int main() {
    u32 GuardaChi = 0;
    u32 MitadNumeroVertices = 0;
    u32 CantidadVertices = 0;

    u32 *OrdenAleatorio = NULL;
    u32 *GuardaOrden = NULL;

    float Probabilidad = 0;
    u32 CG = 0;
    u32 GC = 0;
    u32 R = 0;
    u32 RAR = 0;

    NimheP G = NuevoNimhe();
    
    if (G != NULL) {
        srand(NumeroDeVertices(G) - NumeroDeLados(G));

        if (Chidos(G)) {
            printf("Grafo Bipartito\n");
        } else {

            printf("Grafo No Bipartito\n");

            GuardaOrden = calloc(NumeroDeVertices(G), sizeof(u32));
            OrdenAleatorio = calloc(NumeroDeVertices(G), sizeof(u32));

            GuardaChi = NumeroDeVertices(G) + 1;
            CantidadVertices = NumeroDeVertices(G);
            MitadNumeroVertices = (NumeroDeVertices(G) + 1) / 2;

            for (u32 n = 0; n < MitadNumeroVertices; n++) {
                OrdenAleatorio[n] = n;
                OrdenAleatorio[CantidadVertices - n - 1] = CantidadVertices - n - 1;
            }

            for (u32 i = 0; i < 10; i++) {
                ReordenMain(OrdenAleatorio, NumeroDeVertices(G));
                OrdenEspecifico(G, OrdenAleatorio);
                Greedy(G);
                printf("coloreo aleatorio numero %d: %d colores\n", i, CantidadDeColores(G));

                if (CantidadDeColores(G) < GuardaChi) {
                    GuardaChi = CantidadDeColores(G);
                    for (u32 n = 0; n < MitadNumeroVertices; n++) {
                        GuardaOrden[n] = OrdenAleatorio[n];
                        GuardaOrden[CantidadVertices - n - 1] = OrdenAleatorio[CantidadVertices - n - 1];
                    }
                }
            }

            OrdenWelshPowell(G);
            Greedy(G);

            printf("\n coloreo con Greedy en WelshPowell:%d colores\n", CantidadDeColores(G));

            if (CantidadDeColores(G) >= GuardaChi) {
                OrdenEspecifico(G, GuardaOrden);
                Greedy(G);
            }

            if (CantidadDeColores(G) == 3) {
                printf("X(G)=3\n");
            } else {

                printf("\n====Comenzando Greedy Iterado 1001 veces====\n\n");

                for (u32 i = 0; i < 1001; i++) {
                    Probabilidad = (float)(rand() % 8 + 1) / 16;

                    if (Probabilidad <= 0.0625) {
                        // 6,25% ReordenAleatorioRestringido.
                        ReordenAleatorioRestringido(G);
                        RAR++;
                    } else if (Probabilidad > 0.0625 && Probabilidad <= 0.1875) {
                        // 12,5% GrandeChico.
                        GrandeChico(G);
                        GC++;
                    } else if (Probabilidad > 0.1875 && Probabilidad < 0.5) {
                        // 31,25% Revierte.
                        Revierte(G);
                        R++;
                    } else if (Probabilidad >= 0.5) {
                        // 50% ChicoGrande.
                        ChicoGrande(G);
                        CG++;
                    }

                    Greedy(G);
                }

                Revierte(G);
                Greedy(G);

                printf("\nMejor coloreo con Greedy iterado 1001 veces: %d colores\n", CantidadDeColores(G));
                printf("(%d CG,%d GC, %d R, %d RAR)\n", CG, GC, R, RAR);
            }
        }

        free(GuardaOrden);
        free(OrdenAleatorio);
    } else {
        printf("Error en formato de entrada.\n");
    }
    
    DestruirNimhe(G);
    return 0;
}
