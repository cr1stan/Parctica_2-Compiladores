#include "dfa.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>


#define EPSILON '\0'

State*crear_nodo(char transicion) {
    State *s = (State *)malloc(sizeof(State));
    s->transiccion = transicion;
    s->visitado = false;
    s->salida1 = NULL;
    s->salida2 = NULL;
    return s;
}

void probar_epsilon_clausura() {
    printf("[PRUEBA] Evaluando epsilon-Closure...\n");
    State *s0 = crear_nodo(EPSILON);
    State *s1 = crear_nodo(EPSILON);
    State *s2 = crear_nodo(EPSILON);
    State *s3 = crear_nodo('a');

    s0->salida1 = s1;
    s1->salida1 = s2;
    s2->salida1 = s1;

    ConjuntoEstados *clausura = crear_conjunto();
    agregar_estado(clausura, s0);

    ConjuntoEstados *resultado = epsilon_closure(clausura);
    assert(resultado->tamano == 3);
    assert(pertenece_al_conjunto(resultado, s0));
    assert(pertenece_al_conjunto(resultado, s1));
    assert(pertenece_al_conjunto(resultado, s2));
    assert(!pertenece_al_conjunto(resultado, s3));

    printf("[PRUEBA] epsilon-Closure pasó exitosamente.\n");
    liberar_conjunto(clausura);
    liberar_conjunto(resultado);
    free(s0);
    free(s1);
    free(s2);
    free(s3);
}

void probar_mover() {
    printf("[PRUEBA] Evaluando move...\n");
    State *s0 = crear_nodo('a');
    State *s1 = crear_nodo('\0');
    State *s2 = crear_nodo('\0');

    s0->salida1 = s1;
    s0->salida2 = s2;

    ConjuntoEstados *conjunto = crear_conjunto();
    agregar_estado(conjunto, s0);

    ConjuntoEstados *resultado = move(conjunto, 'a');
    assert(resultado->tamano == 2);
    assert(pertenece_al_conjunto(resultado, s1));
    assert(!pertenece_al_conjunto(resultado, s2));

    ConjuntoEstados *resultado_vacio = move(conjunto, 'b');
    assert(resultado_vacio->tamano == 0);

    printf("[PRUEBA] move pasó exitosamente.\n");
    liberar_conjunto(conjunto);
    liberar_conjunto(resultado);
    free(s0);
    free(s1);
    free(s2);
}
void probar_nfa_to_dfa() {
    printf("[PRUEBA] Evaluando nfa_to_dfa...\n");
    printf("\n--- ESTRUCTURA BASE DEL NFA (ANTES) ---\n");
    printf("Estado Inicial: q0 (Transiciones: Epsilon -> q1, Epsilon -> q2)\n");
    printf("Estado q1: Transicion 'a' -> q3\n");
    printf("Estado q2: Transicion 'b' -> q3\n");
    printf("Estado Final: q3\n");
    State *q0=crear_nodo(EPSILON);
    State *q1=crear_nodo('a');
    State *q2=crear_nodo('b');
    State *q3=crear_nodo(EPSILON);
    q0->salida1=q1;
    q0->salida2=q2;
    q1->salida1=q3;
    q2->salida1=q3;

    nfa aut={q0,q3};
    char alfabeto[] = {'a', 'b'};
    DFA *dfa = nfa_to_dfa(&aut, alfabeto, 2);
    imprimir_tabla_dfa(dfa, alfabeto, 2);
    assert(dfa != NULL);
    assert(dfa->estados_inicial -> es_final == false);
    DFAState *trans_a = dfa->estados_inicial->transiciones[(unsigned char)'a'];
    DFAState *trans_b = dfa->estados_inicial->transiciones[(unsigned char)'b'];
    assert(trans_a != NULL);
    assert(trans_b != NULL);
    assert(trans_a->es_final == true);
    assert(trans_b->es_final == true);
    printf("[PRUEBA] nfa_to_dfa paso exitosamente (%d estados).\n", dfa->num_estados);
    free_dfa(dfa);
    free(q0);
    free(q1);
    free(q2);
    free(q3);
}
int main() {
    printf("--- INICIANDO SUITE DE PRUEBAS UNITARIAS ---\n\n");
    probar_epsilon_clausura();
    //probar_mover();
    probar_nfa_to_dfa();
    printf("\n--- TODAS LAS PRUEBAS PASARON CON ÉXITO ---\n");
    return 0;
}
    