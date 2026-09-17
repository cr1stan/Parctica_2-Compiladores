/**
 * @file nfa.h
 * @brief Definición de estructuras y funciones para la manipulación de autómatas finitos no deterministas (NFA).
 * @author 
 * @author 
 * @author 
 * @date 
 */
#ifndef NFA_H
#define NFA_H
#include <stdbool.h>
#include "regex.h"

/**
 * @brief Estructura que representa un estado en un NFA.
 * @param transiccion El símbolo de transición del estado {a,b,c....,Epsilon}.
 * @param salida1 Puntero al primer estado de salida, usado para transiciones epsilon y normales.
 * @param salida2 Puntero al segundo estado de salida, usado para transiciones epsilon y bifurcaciones.
 * 
 */
typedef struct State {
    char transiccion;
    bool visitado;
    struct State *salida1;
    struct State *salida2;
}State;

/**
 * @brief Estructura que representa un NFA.
 * @param estado_inicial Puntero al estado inicial del NFA.
 * @param estado_final Puntero al estado final del NFA.
 */
typedef struct {
    State *estado_inicial;
    State *estado_final;
} nfa;

// Funciones para manipular NFA
nfa regex_to_nfa(regex r);
int match_nfa(nfa n, const char *str, int len);
void free_nfa(nfa *n);
bool save_nfa(const nfa *n, const char *output_path);

#endif