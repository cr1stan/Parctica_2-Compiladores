/**
 * @file nfa.c
 * @brief Implementación de funciones para la construcción y manipulación de autómatas finitos no deterministas (NFA) a partir de expresiones regulares.
 * @author 
 * @author 
 * @author 
 * @date 
 */

// Bibliotecas Basicas
#include "nfa.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Definimos el tamaño máximo de la pila y el símbolo de transición epsilon
#define MAX_STACK 1000
#define EPSILON '\0'

// Definimos el tamaño máximo de estados que puede tener un NFA
#define MAX_STATES 1000

/**
 * @brief Funcion axuliar para crear un nuevo estado en el NFA.
 * @param transicion El símbolo de transición del estado.
 * @param salida1 La primera salida del estado.
 * @param salida2 La segunda salida del estado.
 * @return Un puntero al nuevo estado creado.
 */
State* crear_estado(char transicion, State* salida1, State* salida2){
    State* s=(State*)malloc(sizeof(State));
    s->visitado=false;
    s->transiccion=transicion;
    s->salida1=salida1;
    s->salida2=salida2;
    return s;
}

// Definimos la pila para almacenar los NFA y el índice del tope de la pila
nfa pila[MAX_STACK];
int top=-1;

// Funciones para manipular la pila de NFA
void push(nfa n){pila[++top]=n;}
nfa pop(){return pila[top--];}

/**
 * @brief Función para convertir una expresión regular en un NFA.
 * @param r La expresión regular a convertir.
 * @return El NFA resultante.
 */

nfa regex_to_nfa(regex r){
    top=-1;
    for (int i = 0; i < r.size; i++)
    {
        char c=r.items[i].value;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')){
            State* estado_final=crear_estado(EPSILON,NULL,NULL);
            State* estado_inicial=crear_estado(c,estado_final,NULL);

            nfa nuevo_nfa={estado_inicial, estado_final};
            push(nuevo_nfa);

        }else if (c=='.'){
            nfa n2= pop();
            nfa n1= pop();

            n1.estado_final->transiccion=EPSILON;
            n1.estado_final->salida1=n2.estado_inicial;

            nfa nuevo_nfa={n1.estado_inicial,n2.estado_final};
            push(nuevo_nfa);

        }else if (c=='|'){
            nfa n2=pop();
            nfa n1=pop();

            State* nuevo_inicio=crear_estado(EPSILON,n1.estado_inicial,n2.estado_inicial);
            State* nuevo_final=crear_estado(EPSILON,NULL,NULL);

            n1.estado_final->transiccion=EPSILON;
            n1.estado_final->salida1=nuevo_final;

            n2.estado_final->transiccion=EPSILON;
            n2.estado_final->salida1=nuevo_final;
            nfa nuevo_nfa={nuevo_inicio,nuevo_final};
            push(nuevo_nfa);

        }else if (c=='*'){
            nfa n=pop();
            State* nuevo_final=crear_estado(EPSILON,NULL,NULL);
            State* nuevo_inicio=crear_estado(EPSILON,n.estado_inicial,nuevo_final);
            n.estado_final->transiccion=EPSILON;
            n.estado_final->salida1=n.estado_inicial;
            n.estado_final->salida2=nuevo_final;

            nfa nuevo_nfa={nuevo_inicio,nuevo_final};
            push(nuevo_nfa);
        }else if (c=='+'){
            nfa n=pop();
            State* nuevo_final=crear_estado(EPSILON, NULL,NULL);
            State* nuevo_inicio=crear_estado(EPSILON,n.estado_inicial, NULL);
            n.estado_final->transiccion=EPSILON;
            n.estado_final->salida1=n.estado_inicial;
            n.estado_final->salida2=nuevo_final;

            nfa nuevo_nfa={nuevo_inicio, nuevo_final};
            push(nuevo_nfa);
        }else if (c=='?'){
            nfa n=pop();
            State* nuevo_final=crear_estado(EPSILON,NULL,NULL);
            State* nuevo_inicio=crear_estado(EPSILON,n.estado_inicial,nuevo_final);
            n.estado_final->transiccion=EPSILON;
            n.estado_final->salida1=nuevo_final;
            nfa nuevo_nfa={nuevo_inicio,nuevo_final};
            push(nuevo_nfa);
        }
    }
    return pop();
}

 /**
 * @brief Función auxiliar para agregar un estado a una posible ruta de solución del NFA,
 *        calculando su cierre-épsilon.
 * @param state El estado que se intenta agregar.
 * @param lista La lista de estados "activos" en la ruta actual.
 * @param n Puntero al número de estados actualmente almacenados en 'lista'.
 * @param marcados Lista auxiliar de todos los estados visitados durante esta llamada,
 *        usada para poder resetear su campo 'visitado' después.
 * @param n_marcados Puntero al número de estados actualmente almacenados en 'marcados'.
 *  
 */
static void agregar_estado(State *s, State **lista, int *n, 
                            State **marcados, int *n_marcados) {
    
    // Caso base para nodos nulos o previamente visitados                            
    if (s == NULL || s->visitado) {
        return;
    }
    
    s->visitado = true;
    marcados[(*n_marcados)++] = s;

    lista[(*n)++] = s;

    if(s->transiccion == EPSILON) {
        agregar_estado(s->salida1, lista, n, marcados, n_marcados);
        agregar_estado(s->salida2, lista, n, marcados, n_marcados);
    }
}

/**
 * @brief Función para verificar si una cadena coincide con un NFA.
 * @param n El NFA a usar para la verificación.
 * @param str La cadena a verificar.
 * @param len La longitud de la cadena.
 * @return true si la cadena coincide con el NFA, false en caso contrario.
 */
int match_nfa(nfa n, const char *str, int len) {
    State *actuales[MAX_STATES];
    State *siguientes[MAX_STATES];
    State *marcados[MAX_STATES];
    int n_actuales = 0, n_siguientes, n_marcados;

    // Cierre-épsilon del estado incial
    n_marcados = 0;
    agregar_estado(n.estado_inicial, actuales, &n_actuales, marcados, &n_marcados);
    for (int k = 0; k < n_marcados; k++) {
        marcados[k]->visitado = false;
    }

    for (int i = 0; i < len; i++) {
        n_siguientes = 0;
        n_marcados = 0;

        for (int j = 0; j < n_actuales; j++) {
            State *s = actuales[j];
            if (s->transiccion == str[i]) {
                agregar_estado(s->salida1, siguientes, &n_siguientes, marcados, &n_marcados);
            }
        }

        for (int k = 0; k < n_marcados; k++) {
            marcados[k]->visitado = false;
        }

        for (int j = 0; j < n_siguientes; j++) { 
            actuales[j] = siguientes[j];
        }

        n_actuales = n_siguientes;

        if (n_actuales == 0) {
            return 0; // no hay camino vivo, por lo que no hay match
        }
    }

    for (int j = 0; j < n_actuales; j++) {
        if (actuales[j] == n.estado_final) {
            return 1; // hay match
        }
    }

    return 0;
}

static void liberar_recursivamente(State *estado) {
    if (estado == NULL || estado->visitado) {
        return;
    }
    estado->visitado = true;

    liberar_recursivamente(estado->salida1);
    liberar_recursivamente(estado->salida2);

    free(estado);
}

/**
 * @brief Función para liberar la memoria de un NFA.
 * @param n El NFA a liberar. 
 */
void free_nfa(nfa *n){
    if (n == NULL) {
        return;
    }
    liberar_recursivamente(n->estado_inicial);
    n->estado_inicial = NULL;
    n->estado_final = NULL;
}

/**
 * @brief Función auxiliar que recorre el NFA de manera recursiva con el objetivo de 
 *        asignar un número único a cada estado y guardarlo como lista".
 * @param s El estado actual a numerar.
 * @param lista Arreglo donde se guardan los estados en el orden que se numeran.
 * @param n Número de estados numerados hasta al momento.
 */
static void numerar_estados(State *s, State **lista, int *n) {
    if (s == NULL || s->visitado) {
        return;
    }
    s->visitado = true;
    lista[(*n)++]=s;
    numerar_estados(s->salida1, lista, n);
    numerar_estados(s->salida2, lista, n);
}

/**
 * @brief Función para guardar un NFA en un archivo.
 * @param n El NFA a guardar.
 * @param output_path La ruta del archivo de salida.
 * @return true si se guardó correctamente, false en caso contrario.
 */
bool save_nfa(const nfa *n, const char *output_path) {
    if (n == NULL || n->estado_inicial == NULL || output_path == NULL) {
        return false;
    }
    
    FILE *f = fopen(output_path, "w");
    if (f == NULL) {
        return false;
    }

    // Numeramos todos los estados alcanzables desde nuestro incial
    State *lista[MAX_STATES];
    int total = 0;
    numerar_estados(n->estado_inicial, lista, &total);

    // Buscamos el índice de nuestro estado final en la lista
    int indice_final = -1;
    for (int i = 0; i < total; i++) {
        if (lista[i] == n->estado_final) {
            indice_final = i;
            break;
        }
    }

    fprintf(f, "ESTADO_INICIAL: 0\n");
    fprintf(f, "ESTADO_FINAL: %d\n", indice_final);
    fprintf(f, "TOTAL_ESTADOS: %d\n", total);

    // Escribimos cada estado con sus transiciones
    for (int i = 0; i < total; i++) {
        State *s = lista[i];

        // 1. Buscamos hacia dónde apunta la salida1 (si existe)
        if (s->salida1 != NULL) {
            for (int j = 0; j < total; j++) {
                if (lista[j] == s->salida1) {
                    if (s->transiccion == EPSILON) {
                        fprintf(f, "%d -EPSILON-> %d\n", i, j);
                    } else {
                        fprintf(f, "%d -%c-> %d\n", i, s->transiccion, j);
                    }
                    break; 
                }
            }
        }

        // 2. Buscamos hacia dónde apunta la salida2 
        if (s->salida2 != NULL) {
            for (int j = 0; j < total; j++) {
                if (lista[j] == s->salida2) {
                    fprintf(f, "%d -EPSILON-> %d\n", i, j);
                    break;
                }
            }
        }
    }
        for (int i = 0; i < total; i++) {
        lista[i]->visitado = false;
        }
        fclose(f);
        return true;
}
