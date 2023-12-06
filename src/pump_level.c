/**
 * @file pump_level.c
 * @brief Sensor de nivel de agua de la bomba de agua
 * @note El sensor dice cuando el nivel de agua es bajo o no 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pump_level.h"

void pump_level_init(uint8_t gpio)
{
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_pull_up(gpio);
}


bool get_pump_level(uint8_t gpio)
{
    return gpio_get(gpio);
}