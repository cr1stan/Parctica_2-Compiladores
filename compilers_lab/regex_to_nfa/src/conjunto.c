#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "dfa.h"

ConjuntoEstados* crear_conjunto(){
    ConjuntoEstados *c= (ConjuntoEstados*) malloc(sizeof(ConjuntoEstados));
    c ->tamano=0;
    c->capacidad=8;
    c->estados= (State**) malloc(sizeof(State*) * c->capacidad);
    return c;
}

bool pertenece_al_conjunto(const ConjuntoEstados *conjunto, const State *s) {
    if (!conjunto || !s) return false;

    for (int i = 0; i < conjunto->tamano; i++) {
        if (conjunto->estados[i] == s) { 
            return true;
        }
    }
    return false;
}


bool agregar_estado(ConjuntoEstados *conjunto, State *s) {
    if (!conjunto || !s) return false;
    if (pertenece_al_conjunto(conjunto, s)) return false; 

    if (conjunto->tamano == conjunto->capacidad) {
        conjunto->capacidad *= 2;
        conjunto->estados = (State**) realloc(conjunto->estados, sizeof(State*) * conjunto->capacidad);
    }

    conjunto->estados[conjunto->tamano++] = s;
    return true;
}


bool conjuntos_iguales(const ConjuntoEstados *a, const ConjuntoEstados *b) {
    if (!a || !b) return false;
    if (a->tamano != b->tamano) return false;

    for (int i = 0; i < a->tamano; i++) {
        if (!pertenece_al_conjunto(b, a->estados[i])) {
            return false;
        }
    }
    return true;
}


void liberar_conjunto(ConjuntoEstados *conjunto) {
    if (!conjunto) return;
    free(conjunto->estados);
    free(conjunto);
}