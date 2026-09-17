/**
 * @file regex.c
 * @brief Implementación para transformar expresiones regulares de formato infijo a posfijo.
 */

#include "regex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Devuelve un número entero que representa la jerarquía del operador.
static int precedencia(char c) {
    switch (c) {
        case '*':
        case '+':
        case '?':
            return 3;
        case '.':
            return 2;
        case '|':
            return 1;
        default:
            return 0;
    }
}

// Verifica si un carácter es un símbolo válido del alfabeto (letra o número).
static bool es_operando(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

/**
 * @brief Paso 1: Inserta el operador explícito '.' de concatenación.
 * Ejemplo: "a(b|c)*d" -> "a.(b|c)*.d"
 */
static char* insertar_concatenacion_explicita(const char *in) {
    int lon = strlen(in);
    if (lon == 0) return strdup("");  //Verifica cadena vacia.

    // Reservar el doble de memoria por si hay que insertar un '.' entre cada carácter.
    char *out = (char*)malloc(sizeof(char) * (lon * 2 + 1));
    int j = 0;  // Índice para escribir dentro del nuevo arreglo 'out'

    for (int i = 0; i < lon; i++) {
        char c1 = in[i];   // Carácter actual que estamos analizando
        out[j++] = c1;   // Copiar el carácter actual al arreglo de salida

        if (i + 1 < lon) {  // Verificamos si existe un siguiente carácter sin rebasar lon.
            char c2 = in[i + 1];

            // Verifica c1 es "final de un bloque": es un operando, o un operador posfijo (*,+,?), o cierra paréntesis ')'
            bool c1_es_final = es_operando(c1) || c1 == '*' || c1 == '+' || c1 == '?' || c1 == ')';
            // Verifica c2 es "inicio de un bloque": es un operando o abre paréntesis '('
            bool c2_es_inicio = es_operando(c2) || c2 == '(';

            // Si se cumplen ambas condiciones, hay una unión implícita e inserta un punto '.'
            if (c1_es_final && c2_es_inicio) {
                out[j++] = '.';  // Inserta la concatenación explícita.
            }
        }
    }
    out[j] = '\0';  // Agregar el carácter nulo al final para cerrar el string.
    return out;
}

/**
 * @brief Paso 2: Convierte la expresión con '.' a notación posfija (Shunting-Yard).
 */
regex parse_regex(const char *s) {
    // 1. Insertar concatenaciones explícitas
    char *exp_format = insertar_concatenacion_explicita(s);
    int lon = strlen(exp_format);

    // Salida temporal y pila de operadores
    char *salida = (char*)malloc(sizeof(char) * (lon + 1));
    char *pila = (char*)malloc(sizeof(char) * (lon + 1));
    int top_pila = -1;   // Indice del tope (-1 significa pila vacía)
    int pos_salida = 0;  // Índice de posición en la salida

    // Recorrer cada carácter de la expresión "exp_format".
    for (int i = 0; i < lon; i++) {
        char c = exp_format[i];

        if (es_operando(c)) {
            // Regla 1: Si es una letra/número, va directo a la salida.
            salida[pos_salida++] = c;
        } else if (c == '(') {
            // Regla 2: Si es un '(', se guarda en la pila
            pila[++top_pila] = c;
        } else if (c == ')') {
            // Regla 3: Si es un ')', desapilar operadores hacia la salida hasta hallar el '('
            while (top_pila >= 0 && pila[top_pila] != '(') {
                salida[pos_salida++] = pila[top_pila--];
            }
            if (top_pila >= 0) top_pila--; // Sacar el '('
        } else {
            // Regla 4: Es un operador (*, +, ?, ., |)
            // Mientras la pila no esté vacía y el operador del tope tenga MAYOR o IGUAL prioridad
            // que el operador actual 'c', sacar el del tope y lo mandamos a la salida.
            while (top_pila >= 0 && precedencia(pila[top_pila]) >= precedencia(c)) {
                salida[pos_salida++] = pila[top_pila--];
            }
            // Meter el operador actual 'c' a la pila
            pila[++top_pila] = c;
        }
    }

    // Vaciar los operadores restantes de la pila
    while (top_pila >= 0) {
        salida[pos_salida++] = pila[top_pila--];
    }
    salida[pos_salida] = '\0';  // Terminar el string de salida

    // 3. Empaquetar el resultado en la estructura `regex`
    regex r;
    r.size = pos_salida;  // Guardar la cantidad total de elementos.
    r.items = (Item*)malloc(sizeof(Item) * r.size);  // Reservar memoria para el arreglo de Items.

    // Copiar carácter por carácter dentro de los struct Item
    for (int i = 0; i < r.size; i++) {
        r.items[i].value = salida[i];
    }

    // Liberar memoria temporal de los arreglos creados con malloc.
    free(exp_format);
    free(salida);
    free(pila);

    return r;  // Devolver la expresión regular procesada en posfijo.
}
