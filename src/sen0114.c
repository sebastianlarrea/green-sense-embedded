/**
 * @file sen0114.c
 * @brief Implementacion del modulo sen0114. Es un sensor de humedad en suelo que hace uso del ADC del RP2040
 * @note valores de humedad: 0 a 2500 aproximadamente 
 * @version 1.0
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "sen0114.h"

struct adc_config {
    uint8_t adc_gpio;
    uint8_t adc_chan_sel;
}adc_cfg;

void sen0114_init(const uint8_t adc_gpio, const uint8_t adc_chan_sel)
{
    /**
     * @brief Inicializa el modulo ADC en el pin deseado
     * @param in: adc_gpio: Pin GPIO a utilizar
     * @param in: adc_chan_sel: Canal ADC a utilizar
    */
    adc_cfg.adc_gpio = adc_gpio;
    adc_cfg.adc_chan_sel = adc_chan_sel;
    adc_init();
    adc_gpio_init(adc_gpio);
    adc_select_input(adc_chan_sel);
}

static uint16_t sen0114_read(void)
{
    /**
     * @brief Lee el valor del sensor
     * @param in: adc_chan_sel: Canal ADC a utilizar
     * @return out: Valor del sensor
    */
    adc_select_input(adc_cfg.adc_chan_sel);
    return adc_read();
}

float sen0114_read_humidity(void)
{
    /**
     * @brief Lee el valor del sensor y lo convierte a porcentaje de humedad
     * @param in: adc_chan_sel: Canal ADC a utilizar
     * @return out: Valor del sensor en porcentaje de humedad
    */
    return (float)sen0114_read() * 100.0 / SEN0114_MAX;
}