/**
 * @file pump_level.c
 * @brief Sensor de nivel de agua de la bomba de agua
 * @note El sensor dice cuando el nivel de agua es bajo o no 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pump_level.h"

void pump_level_init(void)
{
    gpio_init(GPIO_PUMP_LEVEL);
    gpio_set_dir(GPIO_PUMP_LEVEL, GPIO_IN);
    gpio_set_irq_enabled_with_callback(GPIO_PUMP_LEVEL, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &pump_level_callback);
}

static void pump_level_callback(uint gpio, uint32_t events)
{
    if (events & GPIO_IRQ_EDGE_RISE)
    {
        printf("IRQ: Nivel de agua alto\n");
    }
    else if (events & GPIO_IRQ_EDGE_FALL)
    {
        printf("IRQ: Nivel de agua bajo\n");
    }
}

bool get_pump_level(void)
{
    return gpio_get(GPIO_PUMP_LEVEL);
}