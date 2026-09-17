#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "dfa.h"
#define EPSILON '\0'
ConjuntoEstados* move(const ConjuntoEstados *T, char a) {
    if (!T) return NULL;
    ConjuntoEstados *R = crear_conjunto();

    for (int i = 0; i < T->tamano; i++) {
        State *s = T->estados[i];
        
        if (s && s->transiccion == a) {
            if (s->salida1 != NULL) {
                agregar_estado(R, s->salida1);
            }
            if (s->salida2 != NULL) {
                agregar_estado(R, s->salida2);
            }
        }
    }
    return R;
}

// Recupera todos los estados alcanzables mediante transiciones epsilon desde un conjunto inicial.
ConjuntoEstados* epsilon_closure(const ConjuntoEstados *T) {
    if (!T) return NULL;

    // 1. C <- T (Inicializar conjunto resultado con los elementos de T)
    ConjuntoEstados *C = crear_conjunto();
    for (int i = 0; i < T->tamano; i++) {
        agregar_estado(C, T->estados[i]);
    }

    // 1. Pila <- inicializar con todos los elementos de T
    int capacidad_pila = (T->tamano > 0) ? T->tamano * 2 : 8;
    int tope = 0;
    State **pila = (State**) malloc(sizeof(State*) * capacidad_pila);

    for (int i = 0; i < T->tamano; i++) {
        pila[tope++] = T->estados[i];
    }

    // 3. mientras Pila != 0 entonces
    while (tope > 0) {
        // 4. t <- desapilar(Pila)
        State *t = pila[--tope];

        // 5. Para todos u que pertenezcan a SN(t, e) entonces
        // En Thompson, las salidas con el símbolo 'epsilon' son salida1 y salida2
        if (t->transiccion == EPSILON) {
            State *vecinos[2] = {t->salida1, t->salida2};

            for (int k = 0; k < 2; k++) {
                State *u = vecinos[k];
                if (u != NULL) {
                    // 6. Si u no pertenece a C entonces
                    if (!pertenece_al_conjunto(C, u)) {
                        // 7. C <- C U {u}
                        agregar_estado(C, u);

                        // 8. apilar(Pila, u)
                        if (tope == capacidad_pila) {
                            capacidad_pila *= 2;
                            pila = (State**) realloc(pila, sizeof(State*) * capacidad_pila);
                        }
                        pila[tope++] = u;
                    }
                    // 9. end if
                }
            }
        }
        // 10. end for
    }
    // 11. end while

    // Liberar la estructura dinámica de la pila auxiliar
    free(pila);

    // 12. return C
    return C;
}

DFA* nfa_to_dfa(const nfa *n, const char *alfabeto, int tam_alfabeto) {

    int indices = 0;
    ConjuntoEstados *pila[100];
    ConjuntoEstados *cola[100];
    DFAState *colaEstados[100];

    int tope_pila = -1; //la pila esta vacia
    int ind_cola = 0;

    DFA *retorno = malloc(sizeof(DFA));

    //Definidion de S0 y se agrega a la cola
    State *inicial = n->estado_inicial;
    ConjuntoEstados *cerradura_inicial = crear_conjunto();
    agregar_estado(cerradura_inicial, inicial);
    cerradura_inicial = epsilon_closure(cerradura_inicial);


    //indice correspondiente el a cola
    DFAState *inicio_DFA = malloc(sizeof(DFAState));;
    inicio_DFA->id = indices;
    indices++; 

    if(pertenece_al_conjunto(cerradura_inicial,n->estado_final)){
        inicio_DFA->es_final = true;
    }

    else{
        inicio_DFA->es_final = false;
    }

    for(int n = 0; n < ALFABETO_SIZE; n++){
        inicio_DFA->transiciones[n] = NULL;
    }

    colaEstados[ind_cola] = inicio_DFA;

    tope_pila++;
    pila[tope_pila] = cerradura_inicial;
    cola[ind_cola] = cerradura_inicial;

    
    ind_cola++;
    
    //mientras la [ila esta vacia]
    while(tope_pila != -1 ){

        ConjuntoEstados *actual = pila[tope_pila];
        tope_pila--;

        int indice_actual = -1;
            for(int j = 0; j < ind_cola; j++){
                if(conjuntos_iguales(cola[j], actual)){
                    indice_actual = j;
                    break;
            }
        }

        DFAState *estado_actual = colaEstados[indice_actual];

        //se recorre el alfabeto en busqueda de nuevos estados
        for(int i = 0; i < tam_alfabeto; i++){

            char caracter = alfabeto[i];
            ConjuntoEstados *actual_alcance_char = crear_conjunto();     
            ConjuntoEstados *alcanzable =  epsilon_closure(move(actual, caracter)); //conjunto de estados
            
            int k = 0;
            while(k < alcanzable->tamano){
                    if(pertenece_al_conjunto(actual_alcance_char, alcanzable->estados[k])){
                        k++;
                        continue;
                    }
                    else{
                        agregar_estado(actual_alcance_char, alcanzable->estados[k]);
                        k++;
                    }
                    
            }


            int encontrado = 0;
            int indice_estado_destino = -1;


            for(int l = 0; l < ind_cola; l++){
                if(conjuntos_iguales(cola[l], actual_alcance_char)){
                    encontrado = 1;
                    indice_estado_destino = l;

                    break;
                }

            }

            if(encontrado == 0){

                tope_pila++;
                pila[tope_pila] = actual_alcance_char;

                DFAState *nuevo = malloc(sizeof(DFAState));;
                nuevo->es_final = pertenece_al_conjunto(actual_alcance_char, n->estado_final);
                nuevo->id = indices++;

                for(int j = 0; j < ALFABETO_SIZE; j++){
                    nuevo->transiciones[j] = NULL;
                }
                
                colaEstados[ind_cola] = nuevo;
                cola[ind_cola] = actual_alcance_char;
                
                estado_actual->transiciones[(unsigned char)caracter] = nuevo;

                ind_cola++;             

            }

            else{
                estado_actual->transiciones[(unsigned char)caracter] = colaEstados[indice_estado_destino];

            }

        }

    }


    //construccion final de dfa
    retorno->num_estados = ind_cola;
    retorno->estados = malloc(ind_cola * sizeof(DFAState *));
    
    for(int s = 0; s< ind_cola; s++){
        retorno->estados[s] = colaEstados[s];
    }

    retorno->estados_inicial = colaEstados[0];


    return retorno;

}

void imprimir_tabla_dfa(const DFA *d, const char *alfabeto, int tam_alfabeto) {
    printf("\n=== TABLA DE TRANSICIONES DEL DFA RESULTANTE ===\n");
    printf("| Estado | Final |");
    for (int i = 0; i < tam_alfabeto; i++) {
        printf("  '%c'  |", alfabeto[i]);
    }
    printf("\n---------------------------------------------\n");

    for (int i = 0; i < d->num_estados; i++) {
        DFAState *s = d->estados[i];
        printf("|   q%-3d |  %s  |", s->id, s->es_final ? " SÍ " : " NO ");
        for (int j = 0; j < tam_alfabeto; j++) {
            char c = alfabeto[j];
            DFAState *dest = s->transiciones[(unsigned char)c];
            if (dest) {
                printf("  q%-3d |", dest->id);
            } else {
                printf("  ---  |");
            }
        }
        printf("\n");
    }
    printf("=============================================\n\n");
}

void free_dfa(DFA *d) {
    if (!d) return;
    if (d->estados_inicial) {
        free(d->estados_inicial);
    }
    free(d);
}
