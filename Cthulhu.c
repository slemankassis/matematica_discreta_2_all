#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "Cthulhu.h"

#define KNUTH 2654435761

////////////////////////////////////////////////////////////////////////////////
// Estructuras.

typedef struct ColaVerticeSt *ColaVerticeP;
typedef struct ColaSimpleSt *ColaSimpleP;

struct NimheSt {  // Estructura para guardar y manipular el grafo.
    VerticeSt *Grafo;  // Arreglo de vertices que guarda el grafo.
    VerticeSt *OrdenNatural; // Arreglo para guardar en Orden natural el grafo.
    u32 n;  // Cantidad vertices.
    u32 m;  // Cantidad lados.
    u32 Chi;  // Numero cromatico obtenido.
};

struct VerticeStt {  // Estructura para guardar y manipular el Vertice.
    u32 Nombre;  // Nombre del vertice (corresponde al archivo).
    u32 Color;  // Color del vertice
    u32 Grado;  // Grado del Vertice.
    u32 CantidadColor; // Cantidad de vertices coloreados con el color actual del vertice.
    VerticeSt *Vecinos;  // Arreglo de vecinos, cada elemento es un vecino del vertice.
};

struct ColaSimpleSt {  // Cola de vertices (se usa en Chidos).
    VerticeSt vertice;  // Puntero a un vertice en el arreglo *Grafo de NimheSt.
    ColaSimpleP siguiente;  // Siguiente elemento en la lista.
};

struct ColaVerticeSt { // Estructura que guarda una cola y un puntero al ultimo elemento
    ColaSimpleP cola;  // Puntero a la cola de vertices
    ColaSimpleP ultimo;  // Puntero al ultimo elemento de la cola
};

void printgrafo(NimheP G) {
    for (u32 i=0; i<G->n; i++) {
        printf("%u,", G->Grafo[i]->Nombre);
    }
}

void printgrafowp(NimheP G) {
    for (u32 i=0; i<G->n; i++) {
        printf("%u,", G->Grafo[i]->Grado);
    }
}

void comprobarwp(NimheP G) {
        for (u32 i=0; i<G->n-1; i++) {
        if (G->Grafo[i]->Grado < G->Grafo[i+1]->Grado) {
            printf("no es decreciente\n");
            break;
        }
    }
}

void printgrafocolor(NimheP G) {
    for (u32 i=0; i<G->n; i++) {
        printf("%u,", G->Grafo[i]->Color);
    }
}

void printgrafocc(NimheP G) {
    for (u32 i=0; i<G->n; i++) {
        printf("%u,", G->Grafo[i]->CantidadColor);
    }
}


////////////////////////////////////////////////////////////////////////////////
// Funciones AUXILIARES de Cthulhu.

// Funciones AUXILIARES para funciones de crear y destruir grafo.
void AgregarElemento(NimheP G, u32 x);
VerticeSt BuscarElemento(NimheP G, u32 Nombre);
int BuscarElementoPosicion(NimheP G, u32 Nombre);
VerticeSt CrearVertice(u32 Nombre);
u32 FuncionHash(u32 k, u32 m);

// Funciones AUXILIARES de vecinos.
void AgregarVecino(VerticeSt list, VerticeSt x);

//Funciones para el uso de cola
ColaVerticeP AgregarCola(ColaVerticeP Lista, VerticeSt x);
ColaVerticeP BorrarPrimero(ColaVerticeP Lista);
VerticeSt DarPrimero(ColaVerticeP Lista);
ColaVerticeP BorrarCola(ColaVerticeP lista);
bool ColaNoEsVacia(ColaVerticeP Lista);

// Funciones AUXILIARES para funciones de coloreo.
NimheP Decolorar(NimheP G);

// Funciones AUXILIARES para funciones de ordenacion.
void ReordenU32(u32 *array, u32 n);
int ComparaNombre(const void *a, const void *b);
int ComparaGrado(const void *a, const void *b);
int ComparaCantidadColorCG(const void *a, const void *b);
int ComparaColor(const void *a, const void *b);
int ComparaCantidadColorGC(const void *a, const void *b);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Funciones de crear y destruir grafo.

/* Funcion que lee un archivo desde stdin y carga el grafo, verifica que el
formato del archivo sea correcto a medida que va leyendo, si el formato es
incorrecto la funcion devuelve NULL.*/
NimheP NuevoNimhe() {
    FILE *file = stdin;  // Puntero tipo File para obtener el stdin.
    NimheP G = NULL;  // Grafo
    char Linea[80];  // String necesario para cargar las lineas de texto una a una del archivo ingresado.
    u32 cont_n = 0, cont_m = 0; // Variables auxiliares para contar los vertices y vecinos creados
    u32 v1 = 0, v2 = 0;  // Variables auxiliares para guardar el nombre de los vertices leidos.
    u32 n = 0, m = 0;  // Variables auxiliares para guardar la cantidad de vertices (n) y cantidad de lados (m).
    bool flag = true;

    // Verifica que el archivo exista.
    if (file != NULL) {

        /* Ciclo que recorre line por linea el archivo que tiene el grafo
        Si la linea no empieza con c debe emepzar con p, caso contrario termina
        y devuelve NULL*/
        while (fgets(Linea, sizeof(Linea), file)) {
            if (Linea[0] == 'c') {
                continue;
            } else if (sscanf(Linea, "p edge %u %u", &n, &m) == 2) {

                G = calloc(1, sizeof(struct NimheSt));
                G->Grafo = calloc(n, sizeof(void *));
                G->OrdenNatural = calloc(n, sizeof(void *));
                G->n = n;
                G->m = m;
                break;
            } else {
                flag = false;
                break;
            }
        }

        /* Ciclo que recorre line por linea el archivo que tiene el grafo
        El ciclo solo inicia si el anterior identifico una linea p edge...
        Si la linea no empieza con e termina el ciclo y devuelve NULL*/
        while (flag && fgets(Linea, sizeof(Linea), file)) {

            if (sscanf(Linea, "e %u %u", &v1, &v2) == 2) {

                cont_m++;
                int VerticePosicion1 = BuscarElementoPosicion(G, v1);
                int VerticePosicion2 = BuscarElementoPosicion(G, v2);

                // Caso 1: v1 y v2, no existen se agregan al arreglo
                if (VerticePosicion1 == -1 && VerticePosicion2 == -1) {
                    AgregarElemento(G, v1);
                    AgregarElemento(G, v2);
                    cont_n = cont_n + 2;

                // Caso 2: v1 no existe, pero si existe v2, se agrega v1 al arreglo.
                } else if (VerticePosicion1 == -1 && VerticePosicion2 != -1) {
                    AgregarElemento(G, v1);
                    cont_n++;
                // Caso 3: v2 no existe, pero si existe v1, se agrega v2 al arreglo.
                } else if (VerticePosicion1 != -1 && VerticePosicion2 == -1) {
                    AgregarElemento(G, v2);
                    cont_n++;
                }

                // Luego de pasar los 3 casos se crea los vecinos entre ellos
                VerticeSt Vert1 = BuscarElemento(G, v1);
                VerticeSt Vert2 = BuscarElemento(G, v2);

                AgregarVecino(Vert1, Vert2);
                AgregarVecino(Vert2, Vert1);

                Vert1 = NULL;
                Vert2 = NULL;
            } else {
                break;
            }
        }

        /* Guarda para corroborar que la cantidad de vertices y vecinos creados
        Sea igual a la identificada en la linea p edge... */
        if (G != NULL && cont_n == G->n && cont_m == G->m) {
            srand(G->n - G->m);
            u32 Rango = (G->n +1) / 2;
            for (u32 i = 0; i < Rango; i++) {
                G->OrdenNatural[i] = G->Grafo[i];
                G->OrdenNatural[G->n-1-i] = G->Grafo[G->n-1-i];
            }
            qsort(G->OrdenNatural, G->n, sizeof(VerticeSt), ComparaNombre);
        } else {
            DestruirNimhe(G);
            G = NULL;
        }
    }
    file = NULL;
    return G;
}

// Funcion para destruir el grafo y liberar la memoria.
int DestruirNimhe(NimheP G) {
    if(G != NULL) {
        for (u32 i = 0; i < G->n; i++) {
            if (G->Grafo[i] != NULL) {
                free(G->Grafo[i]->Vecinos);
            }
            free(G->Grafo[i]);
            G->Grafo[i] = NULL;
        }
        free(G->Grafo);
        free(G->OrdenNatural);
        free(G);
        G = NULL;
    }
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Funciones AUXILIARES para funciones de crear y destruir grafo.

// Funcion para crear un nuevo vertice.
VerticeSt CrearVertice(u32 Nombre) {
    VerticeSt Nuevo;
    Nuevo = calloc(1, sizeof(struct VerticeStt));
    Nuevo->Nombre = Nombre;
    Nuevo->Color = 0;
    Nuevo->Grado = 0;
    Nuevo->CantidadColor = 0;
    Nuevo->Vecinos = calloc(0, sizeof(void *));
    return Nuevo;
}

/* Funcion hash para implementar la tabla hash, en este caso es de
multiplicacion, recibe un valor k (nombre del vertice), lo multiplica por un
numero primo (KNUTH) y obtiene el resto de haber dividido el producto por m
(cantidad de vertices, tamaño del arreglo).*/
u32 FuncionHash(u32 k, u32 m) {
    u32 k2 = k;
    u32 m2 = m;
    u32 ret = (k2 * KNUTH) % m2;
    if (ret > m2) {
        ret = ret % m2;
    }
    return (u32)ret;
}

/* Busca un elemento x (Vertice) en G(Grafo) y devuelve un puntero a su
estructura; en caso de no encontrarlo devuelve NULL, se implementa el
concepto de busqueda en una tabla de hash para hacer la busqueda mas rapido.*/
VerticeSt BuscarElemento(NimheP G, u32 Nombre) {
    u32 ValorFuncion = 0;
    ValorFuncion = FuncionHash(Nombre, G->n);
    while (1) {
        if (G->Grafo[ValorFuncion] == NULL) {
            break;
        } else if (G->Grafo[ValorFuncion]->Nombre == Nombre) {
            return G->Grafo[ValorFuncion];
            break;
        } else {
            ValorFuncion++;
            if (ValorFuncion >= G->n) {
                ValorFuncion = 0;
            }
        }
    }
    return NULL;
}

/* Busca un elemento x (Vertice) en G(Grafo) y devuelve el valor de la
posicion donde esta el elemento guardado; en caso de no encontrarlo devuelve 
-1, se implementa el concepto de busqueda en una tabla de hash para hacer
 la busqueda mas rapido.*/
int BuscarElementoPosicion(NimheP G, u32 Nombre) {
    u32 ValorFuncion = 0;
    ValorFuncion = FuncionHash(Nombre, G->n);
    while (1) {
        if (ValorFuncion < G->n && G->Grafo[ValorFuncion] == NULL) {
            break;
        } else if (G->Grafo[ValorFuncion]->Nombre == Nombre) {
            return ValorFuncion;
            break;
        } else {
            ValorFuncion++;
            if (ValorFuncion >= G->n) {
                ValorFuncion = 0;
            }
        }
    }
    return -1;
}

/* Agrega un elemento al arreglo de vertices del grafo, recibe el nombre del
vertice y le otorga una posicion en el arreglo, una vez obtenida la posicion
crea la estructura del vertice(VerticeSt) y lo agrega en el arreglo.*/
void AgregarElemento(NimheP G, u32 Nombre) {
    u32 ValorFuncion = 0;
    ValorFuncion = FuncionHash(Nombre, G->n);
    while (1) {
        if (G->Grafo[ValorFuncion] == NULL) {
            G->Grafo[ValorFuncion] = CrearVertice(Nombre);
            break;
        } else if (G->Grafo[ValorFuncion]->Nombre == Nombre) {
            break;
        } else {
            ValorFuncion++;
            if (ValorFuncion >= G->n) {
                ValorFuncion = 0;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Funciones AUXILIARES de vecinos.

/* Funcion para agregar los vecinoes al vertice.*/
void AgregarVecino(VerticeSt list, VerticeSt x) {
    list->Grado++;
    list->Vecinos = realloc(list->Vecinos, list->Grado * sizeof(void *));
    list->Vecinos[list->Grado - 1] = x;
}

/* Lista enlazada para implementar Chidos, en este caso la lista
representa una cola.*/
ColaVerticeP AgregarCola(ColaVerticeP Lista, VerticeSt x) {

    if (Lista == NULL) {
        Lista = calloc(1,sizeof(struct ColaVerticeSt));
    }

    ColaSimpleP nuevo = calloc(1, sizeof(struct ColaSimpleSt));
    nuevo->vertice = x;
    nuevo->siguiente = NULL;

    if (Lista->cola != NULL) {
        Lista->ultimo->siguiente = nuevo;
        Lista->ultimo = Lista->ultimo->siguiente;
    } else {
        Lista->cola = nuevo;
        Lista->ultimo = nuevo;
    }
    return Lista;
}

/* Borrar primer elemento de la cola.*/
ColaVerticeP BorrarPrimero(ColaVerticeP Lista) {
    ColaSimpleP auxiliar = Lista->cola;
    Lista->cola = Lista->cola->siguiente;
    auxiliar->siguiente = NULL;
    auxiliar->vertice = NULL;
    free(auxiliar);

    return Lista;
}

/* Dar primer elmento de la colar.*/
VerticeSt DarPrimero(ColaVerticeP Lista) {
    VerticeSt primero = NULL;
    if (Lista != NULL && Lista->cola != NULL) {
        primero = Lista->cola->vertice;
    }
    return primero;
}

/* Destruir la cola.*/
ColaVerticeP BorrarCola(ColaVerticeP Lista) {
    ColaSimpleP AuxUno = NULL;
    ColaSimpleP Siguiente = NULL;

    if (Lista != NULL) {
        Lista->ultimo = NULL;

        if (Lista->cola != NULL) {
            AuxUno = Lista->cola;
            while (AuxUno != NULL) {
                Siguiente = AuxUno->siguiente;
                free(AuxUno);
                AuxUno = Siguiente;
            }
            Siguiente = NULL;
            AuxUno = NULL;
        }
        Lista->cola = NULL;
    }

    free(Lista);
    Lista = NULL;
    return Lista;
}

// Funcion que devuelve un valor booleano si la cola esta vacia o no
bool ColaNoEsVacia(ColaVerticeP Lista) {
    bool respuesta = false;
    if (Lista != NULL && Lista->cola != NULL) {
        respuesta = true;
    }
    return respuesta;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Funciones de vertices.

u32 ColorDelVertice(VerticeSt x) {
    return x->Color;
}

u32 GradoDelVertice(VerticeSt x) {
    return x->Grado;
}

u32 NombreDelVertice(VerticeSt x) {
    return x->Nombre;
}

void ImprimirVecinosDelVertice(VerticeSt x) {
    u32 rango = x->Grado;
    VerticeSt *vecino = x->Vecinos;

    for (u32 i = 0; i < rango; i++) {
        if (i < rango - 1) {
            printf("%d, ", vecino[i]->Nombre);
        } else {
            printf("%d.\n", vecino[i]->Nombre);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Funciones para obtener informacion del grafo.

u32 NumeroDeVertices(NimheP G) {
    return G->n;
}

u32 NumeroDeLados(NimheP G) {
    return G->m;
}

u32 NumeroVerticesDeColor(NimheP G, u32 i) {
    u32 Contador = 0;
    if (G != NULL) {
        for (u32 n = 0; n < G->n; n++) {
            if (G->Grafo[n]->Color == i) {
                Contador++;
            }
        }
    }
    return Contador;
}

u32 ImprimirVerticesDeColor(NimheP G, u32 i) {
    u32 Contador = 0;
    if (G != NULL) {
        if (NumeroVerticesDeColor(G, i) != 0) {
            printf("Vertices de Color %d: ", i);
            for (u32 i = 0; i < G->n; i++) {
                if (G->Grafo[i]->Color == i) {
                    if (i != G->n - 1) {
                        printf("%d, ", G->Grafo[i]->Nombre);
                    } else {
                        printf("%d.\n", G->Grafo[i]->Nombre);
                    }
                }
            }
        } else {
            printf("No hay vertices de color %d\n", i);
        }
    }
    return Contador;
}

u32 CantidadDeColores(NimheP G) {
    return G->Chi;
}

VerticeSt IesimoVerticeEnElOrden(NimheP G, u32 i) {
    return G->Grafo[i];
}

VerticeSt IesimoVecino(NimheP G, VerticeSt x, u32 i) {
    if (G != NULL && x != NULL) {
        if (i > 0 &&  i < x->Grado) {
            return x->Vecinos[i - 1];
        }
    }
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Funciones de coloreo.

/* Funcion dada en clases. Determina si un grafo tiene numero cromatico 2
(bipartito), en caso positivo devuelve 1, caso negativo devuelve 0.*/
int Chidos(NimheP G) {
    ColaVerticeP Cola = NULL;  // Se inicializa una cola para ir guardando los vertices que se van leyendo.
    u32 VerticesColoreados = 0;  // Contador de vertices que ya se han coloreado.
    VerticeSt p;  // Puntero auxiliar para ir guardando los vertices y operar sobre ellos.
    p = NULL;

    G = Decolorar(G);
    for (u32 i = 0; VerticesColoreados < G->n; i++) {
        if (G->Grafo[i]->Color == 0) { // Obtenemos el primer vertice no coloreado
            G->Grafo[i]->Color = 1; // Le damos el color 1
            VerticesColoreados++; // Incremetamos el contador
            Cola = AgregarCola(Cola, G->Grafo[i]); // Cargamos el vertice en la lista

            while (ColaNoEsVacia(Cola)) {//se queda en este while
                p = DarPrimero(Cola); // Obtenemos el primer elemeneto de la cola
                // Borramos el primer elemento de la cola para no volver a llamarlo
                Cola = BorrarPrimero(Cola);
                for (u32 i = 0; i < p->Grado; i++) {  // Recorremos los vecinos del vertice
                    if (p->Vecinos[i]->Color == 0) {  // Si no tiene Color es candidato a entrar en la cola
                        p->Vecinos[i]->Color = 3 - p->Color;  // Asigna Color
                        Cola = AgregarCola(Cola, p->Vecinos[i]); // Cargamos el vertice en la lista
                        VerticesColoreados++; // Incrementamos contador
                    }

                    // En caso de tener el mismo color que el vecino salimos y retornamos 0

                    if (p->Color == p->Vecinos[i]->Color && p->Color != 0) {
                        p = NULL;
                        Cola = BorrarCola(Cola);
                        Cola = NULL;
                        return 0;
                    }
                }
            }
        }
    }

    p = NULL;
    Cola = BorrarCola(Cola);
    Cola = NULL;

    // En caso de exito, asignamos Chi igual a 2, retornamos 1
    G->Chi = 2;
    return 1;
}

/* La idea usada para resolver de "buena" forma greedy es, escanear los colores
de los vecinos del vertice que se esta por colorear, guardar que colores estan
presentes en los vecinos y asignar al vertice el primer color ausente, en caso
de que todos esten presentes asignar el color mas grande 
posible (cantidad de vertices - 1).*/
u32 Greedy(NimheP G) {

    /* Inicializamos algunos datos del Grafo y variables auxiliares.*/
    G->Chi = 1;
    u32 Rango = G->n;
    u32 RangoVecinos = 0;
    u32 MitadRangoV = 0;
    u32 *Presentes = NULL;
    VerticeSt *Vecinos = NULL;

    /* Este arreglo se usara guardar los colores presentes
    de los vecinos del arreglo.*/
    Presentes = calloc(Rango, sizeof(u32));

    /* Decoloramos el grafo, esto es todos los vertices con
    color igual a cero.*/
    G = Decolorar(G);

    /* Ciclo para leer todos los vertices del arreglo.*/
    for (u32 i = 0; i < Rango; i++) {

        /* Variables para implementar la idea de "desenrollado de ciclos" 
        en este caso si es posible en el ciclo, recorremos el grafo de 
        0 a N y de N-1 a 0, de esta forma hacemos N/2 comparaciones.*/
        RangoVecinos = G->Grafo[i]->Grado;
        MitadRangoV = (RangoVecinos + 1) / 2;
        Vecinos = G->Grafo[i]->Vecinos;

        /* Ciclo para guardar todos los colores de los vecinos
        en el arreglo Presentes.*/
        for (register u32 x = 0; x < MitadRangoV; x++) {
            Presentes[Vecinos[x]->Color] = i+1;
            Presentes[Vecinos[RangoVecinos - x - 1]->Color] = i+1;
        }

        /* Se asignam el color mas grande posible, la cantidad de vecinos + 1
        en caso de no haber colores ausente entre los vecinos, este sera
        el color del vertice.*/
        G->Grafo[i]->Color = RangoVecinos + 1;

        /* Ciclo para buscar el primer color ausente entre los vecinos,
        inicia en 1 dado que el color 0 no interesa, sale del ciclo
        al encontrar un color ausente o haber recorrido todo el arreglo
        Presentes.*/
        for (u32 x = 1; x < RangoVecinos + 1; x++) {
            if (Presentes[x] != i+1) {
                G->Grafo[i]->Color = x;
                break;
            }
        }
        /* Actualiza el valor de Chi en el Grafo, siempre se guarda el 
        mas grande.*/
        if (G->Grafo[i]->Color > G->Chi) {
            G->Chi = G->Grafo[i]->Color;
        }
    }

    free(Presentes);
    return G->Chi;
}

////////////////////////////////////////////////////////////////////////////////
// Funciones AUXILIARES de coloreo.

/* Funcion que recibe un grafo G y asigna color 0 a todos los vertices.*/
NimheP Decolorar(NimheP G) {
    u32 Rango = ((G->n) + 1) / 2;
    if (G != NULL) {
        for (u32 x = 0; x < Rango; x++) {
            G->Grafo[x]->Color = 0;
            G->Grafo[G->n - x - 1]->Color = 0;
        }
    }
    return G;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Funciones de ordenacion.

/* Ordena el arreglo usando la funcion de liberar qsort y una funcion
especifica para que compare los nombres.*/
void OrdenNatural(NimheP G) {
    u32 Rango = (G->n +1) / 2;
    for (u32 i = 0; i < Rango; i++) {
        G->Grafo[i] = G->OrdenNatural[i];
        G->Grafo[G->n-1-i] = G->OrdenNatural[G->n-1-i];
    }
}

/* Ordena el arreglo usando la funcion de librear qsort y una funcion
especifica para que compare el grado de los vertices.*/
void OrdenWelshPowell(NimheP G) {
    qsort(G->Grafo, G->n, sizeof(VerticeSt), ComparaGrado);
}

/* La idea para resolver esta funcion, es agrupar los vertices por color en un 
arreglo de colas de vertices, y copiarlos por color segundo 
un orden aleatorio.*/
void ReordenAleatorioRestringido(NimheP G) {

    // Iterador para recorrer el Grafo de G->Grafo
    u32 x = 0;

    ColaVerticeP ColaVertices = NULL;

    // Arreglo de colas de vertices, cantidad de colas igual a colores usados(Chi)
    ColaVerticeP *ColaVerticesPorColor = calloc(G->Chi + 1, sizeof(struct ColaVerticeSt));
    u32 *Colores = calloc(G->Chi, sizeof(u32));

    /* Carga los colores usados en el grafo en un arreglo auxiliar Colores.*/
    for (u32 i = 0; i < G->Chi; i++) {
        Colores[i] = i+1;
    }
    /* Ordena de forma "aleatoria" los colores presentes en el grafo.*/
    ReordenU32(Colores, G->Chi);

    // Carga todos los vertices separandolos por color, una cola de vertices por color
    for(u32 x = 0; x < G->n; x++) {
        ColaVerticesPorColor[G->Grafo[x]->Color] =
            AgregarCola(ColaVerticesPorColor[G->Grafo[x]->Color], G->Grafo[x]);
    }

    // Ciclo donde se realiza el ordenamiento
    for (u32 i = 0; i < G->Chi; i++) {

        // Carga en ColaVertices la cola de vertices perteneciente al color Colores[i]
        ColaVertices = ColaVerticesPorColor[Colores[i]];

        // Recorremos la cola de vertices y vamos cargando cada elemento de la cola en el
        // arreglo Grafo, es decir el nuevo ordenamiento
        // Cargamos el elemento y lo borramos de la cola
        while(ColaNoEsVacia(ColaVertices)) {
            G->Grafo[x] = DarPrimero(ColaVertices);
            ColaVertices = BorrarPrimero(ColaVertices);
            x++;
        }

        free(ColaVerticesPorColor[Colores[i]]);
    }

    free(ColaVerticesPorColor);
    free(Colores);
    Colores = NULL;
    ColaVerticesPorColor = NULL;

}

/* Para las funciones GrandeChico y ChicoGrande se uso la misma idea para
resolverlo, en el primero ciclo de las funciones se cuenta la cantidad presente
de cada color en el grafo y se lo almacena en Colores, en el segundo ciclo,
actualiza la variable CantidadColor de cada vertice con la cantidad presente del
color en el grafo, por ejemplo si usa 4 veces el color "1" y el vertice tiene el
color 1, CantidadColor sera igual a 4. Por ultimo ordena los vertices del
arreglo usando qsort y una funcion para comparar de menor a mayor segun
corresponda.*/
void GrandeChico(NimheP G) {

    u32 Rango = (G->n + 1) /2;
    // Ordenar por colores
    qsort(G->Grafo, G->n, sizeof(VerticeSt), ComparaColor);

    // Contar la cantidad de cada color presente
    u32 *Colores = calloc(G->Chi + 1, sizeof(u32));
    for (u32 i = 0; i < G->n; i++) {
        Colores[G->Grafo[i]->Color]++;
    }
    // Asigna la cantidad de color presente a cada vertice respectivamente
    for (u32 i = 0; i < Rango; i++) {
        G->Grafo[i]->CantidadColor = Colores[G->Grafo[i]->Color];
        G->Grafo[G->n - 1 - i]->CantidadColor = Colores[G->Grafo[G->n - 1 - i]->Color];
    }

    // Ordenar los vertices por cantidad de color respectivamente
    qsort(G->Grafo, G->n, sizeof(VerticeSt), ComparaCantidadColorGC);

    free(Colores);
    Colores = NULL;
}

void ChicoGrande(NimheP G) {

    u32 Rango = (G->n + 1) /2;
    // Ordenar por colores
    qsort(G->Grafo, G->n, sizeof(VerticeSt), ComparaColor);

    // Contar la cantidad de cada color presente
    u32 *Colores = calloc(G->Chi + 1, sizeof(u32));
    for (u32 i = 0; i < G->n; i++) {
        Colores[G->Grafo[i]->Color]++;
    }
    // Asigna la cantidad de color presente a cada vertice respectivamente
    for (u32 i = 0; i < Rango; i++) {
        G->Grafo[i]->CantidadColor = Colores[G->Grafo[i]->Color];
        G->Grafo[G->n - 1 - i]->CantidadColor = Colores[G->Grafo[G->n - 1 - i]->Color];
    }

    // Ordenar los vertices por cantidad de color respectivamente
    qsort(G->Grafo, G->n, sizeof(VerticeSt), ComparaCantidadColorCG);

    free(Colores);
    Colores = NULL;
}

/* Ordena el arreglo usando la funcion de liberar qsort y una funcion especifica
para que compare el color de cada vertice.*/
void Revierte(NimheP G) {
    qsort(G->Grafo, G->n, sizeof(VerticeSt), ComparaColor);
}

/* Funcion que ordena los vertices segun un orden ingresado por el usuario, 
recibe un grafo (G) y un orden (x) en un arreglo de u32.*/

void OrdenEspecifico(NimheP G, u32* x) {
    /* Arreglo auxiliar que sera una copia del orden ingresado.*/
    u32 Rango = G->n;
    u32 MitadRango = (Rango + 1) / 2;
    u32 *Aux = calloc(Rango + 1, sizeof(u32));

    for (u32 i = 0; i < MitadRango; i++) {
        if (x[i] >= Rango || x[Rango - i - 1] >= Rango) {
            goto salir;
        }
        Aux[x[i]] = 1;
        Aux[x[Rango - i - 1]] = 1;
    }

    /* Este ciclo corrobora que el orden ingresado por el usuario sea del
    formato correcto */
    for (u32 i = 0; i < MitadRango; i++) {
        if (Aux[i] != 1 && Aux[Rango - i - 1] != 1) {
            goto salir;
        }
    }

    /* En este ciclo se cambia el orden, leemos desde VerticeAux el dato
    guardado en la posicion x[i] y lo asignamos en G->Orden.*/
    for (u32 i = 0; i < Rango; i++) {
         G->Grafo[i] = G->OrdenNatural[x[i]];
    }

    salir:
        free(Aux);
}

////////////////////////////////////////////////////////////////////////////////
// Funciones AUXILIARES para funciones de ordenacion.

/* Son usadas cuando usamos qsort, cada una es especifica para un caso de
ordenacion.*/

/* Funcion que compara el nombre de dos vertices de menor a mayor.*/
int ComparaNombre(const void *a, const void *b) {
    VerticeSt p = (VerticeSt)*(u32*)a;
    VerticeSt q = (VerticeSt)*(u32*)b;
    if (p->Nombre < q->Nombre) {
        return -1;
    } else if (p->Nombre > q->Nombre) {
        return 1;
    }
    return 0;
}

/* Funcion que compara el grado de dos vertices de menor a mayor*/
int ComparaGrado(const void *a, const void *b) {
    VerticeSt p = (VerticeSt)*(u32*)a;
    VerticeSt q = (VerticeSt)*(u32*)b;
    if (p->Grado < q->Grado) {
        return 1;
    } else if (p->Grado > q->Grado) {
        return -1;
    }
    return 0;
}

/* Funcion que compara el color de dos vertices de mayor a menor.*/
int ComparaColor(const void *a, const void *b) {
    VerticeSt p = (VerticeSt)*(u32*)a;
    VerticeSt q = (VerticeSt)*(u32*)b;
    if (p->Color < q->Color) {
        return 1;
    } else if (p->Color > q->Color) {
        return -1;
    }
    return 0;
}

/* Funcion que compara la cantidad de colores que guarda cada vertice
de menor a mayor.*/
int ComparaCantidadColorGC(const void *a, const void *b) {
    VerticeSt p = (VerticeSt)*(u32*)a;
    VerticeSt q = (VerticeSt)*(u32*)b;
    if (p->CantidadColor < q->CantidadColor) {
        return 1;
    } else if (p->CantidadColor > q->CantidadColor) {
        return -1;
    }
    return 0;
}

/* Funcion que compara la cantidad de colores que guarda cada vertice
de mayor a menor.*/
int ComparaCantidadColorCG(const void *a, const void *b) {
    VerticeSt p = (VerticeSt)*(u32*)a;
    VerticeSt q = (VerticeSt)*(u32*)b;
    if (p->CantidadColor < q->CantidadColor) {
        return -1;
    } else if (p->CantidadColor > q->CantidadColor) {
        return 1;
    }
    return 0;
}

/* Dos funciones que ordenan de forma aletoria un arreglo
la primera para arreglos de tipo VerticeSt, la segunda
para arreglos de tip u32.*/

void ReordenU32(u32 *array, u32 n) {
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
