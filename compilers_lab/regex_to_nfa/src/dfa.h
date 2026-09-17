#ifndef DFA_H
#define DFA_H

#include <stdbool.h>
#include "nfa.h"

#define ALFABETO_SIZE 256

typedef struct DFAState{
    int id;
    bool es_final;
    struct DFAState *transiciones[ALFABETO_SIZE];
    
}DFAState;

typedef struct {
    DFAState **estados;
    int num_estados;
    DFAState *estados_inicial;
} DFA;

typedef struct {
    State **estados;
    int tamano;
    int capacidad;
} ConjuntoEstados;

// Funciones de apoyo
ConjuntoEstados* crear_conjunto();
bool agregar_estado(ConjuntoEstados *conjunto, State *s); 
bool pertenece_al_conjunto(const ConjuntoEstados *conjunto, const State *s);
bool conjuntos_iguales(const ConjuntoEstados *a, const ConjuntoEstados *b);
void liberar_conjunto(ConjuntoEstados *conjunto);
void imprimir_tabla_dfa(const DFA *d, const char *alfabeto, int tam_alfabeto);
ConjuntoEstados* move(const ConjuntoEstados *T, char a);
ConjuntoEstados* epsilon_closure(const ConjuntoEstados *T);
DFA* nfa_to_dfa(const nfa *n, const char *alfabeto, int tam_alfabeto);
void free_dfa(DFA *d);

#endif