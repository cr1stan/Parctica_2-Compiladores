/**
 * @file regex.h
 * @brief Definición de estructuras y funciones para la manipulación de expresiones regulares
 * @author 
 * @author 
 * @author 
 * @date 
 */
#ifndef REGEX_H
#define REGEX_H
/**
 * @brief Estructura que representa un elemento de una expresión regular.
 * @param value El valor del elemento (carácter).
 */
typedef struct {
    char value;
} Item;
/**
 * @brief Estructura que representa una expresión regular.
 * @param items Puntero a un arreglo de elementos de la expresión regular.
 * @param size El tamaño del arreglo de elementos.
 */
typedef struct {
    Item *items;
    int size;
} regex;
// Función para analizar una cadena de expresión regular y convertirla en una estructura regex
regex parse_regex(const char *regex_str);

#endif