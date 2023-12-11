/**
 * @file main.c
 * @brief Archivo principal del software de la aplicación: green sense
 * @authors Sebastian Larrea Henao, Yonathan Lopez Mejia
*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "app.h"


int main(void)
{
    /**
     * @brief Función principal del programa, inicializa modulos de la pi pico y llama a la función app_main
    */
    stdio_init_all();
    
    app_main();
    
    return 0;
}

