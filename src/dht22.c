#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "dht22.pio.h"
#include "dht22.h"
#include "stdint.h"

PIO pio; /*!< Instancia del PIO*/
uint sm; /*!< State machine del PIO*/
dht_data dht_read_data; /*!< Estructura para almacenar los datos del DHT22*/

void dht22_init(uint8_t pio_code, uint8_t pin) {
    /**
     * @brief Configura el modulo PIO para el DHT22
     * @param pio_code: 0 pio0 ó 1 pio1
     * @param pin: Pin del PIO
    */
    pio = pio_code ? pio1 : pio0; /*!< Obtiene la instancia del PIO*/
    uint offset = pio_add_program(pio, &dht22_program); /*!< Carga el programa en el pio*/
    sm = pio_claim_unused_sm(pio, true); /*!< Reclama un state machine libre*/
    pio_gpio_init(pio, pin); /*!< Inicializa el pin del PIO*/
    pio_sm_config c = dht22_program_get_default_config(offset); /*!< Obtiene la configuracion por defecto del programa*/
    sm_config_set_clkdiv_int_frac(&c, PIO_CLKDIV, 0); /*!< Configura el divisor de reloj, cada instruccion correra a unos 2us*/
    sm_config_set_set_pins(&c, pin, 1); /*!< Establece el pin en la configuracion del state machine*/
    sm_config_set_in_pins(&c, pin); /*!< Establece el pin de entrada en la configuracion del state machine*/
    sm_config_set_in_shift(&c, false, true, 32); /*!< Los datos no necesitan shift, se activa el autopush cuando se hayan leido 32 datos*/
    pio_sm_init(pio, sm, offset, &c); /*!< Inicializa el state machine*/
    pio_sm_set_enabled(pio, sm, true); /*!< Habilita el state machine*/

}

void dht22_restart(void) {
    /**
     * @brief Deshabilita el state machine y libera el PIO
    */
    pio_sm_restart(pio, sm); /*!< Reinicia el state machine*/
}

bool dht22_read(void)
{
    /**
     * @brief Lee los datos del DHT22 byte a byte
     * @return bool: true si hubo un error en la lectura, false si no hubo errores
    */
    
    pio_sm_put_blocking(pio, sm, START_DELAY); /*!< Inicia la lectura, se escribe 500 en el registro OSR*/
    uint8_t data[5]; /*!< Arreglo para almacenar los datos*/
    (*(uint32_t*)data) = pio_sm_get_blocking(pio, sm); /*!< Lee los primeros 32 bits de datos*/
    
    data[4] = pio_sm_get_blocking(pio, sm); /*!< Obtiene el byte de checksum, 5to en el arreglo*/
    dht_read_data.error = (data[4] != (data[3] + data[2] + data[1] + data[0])); /*!< Comprueba si hubo un error en la lectura*/
    dht_read_data.humidity = (data[3] << 8 | data[2]) / 10.0f; /*!< Calcula la humedad relativa conforme a lo indicado en la hoja de datos*/
    dht_read_data.temperature = ((data[1] & 0x7F) << 8 | data[0]) / 10.0f; /*!< Calcula la temperatura conforme a lo indicado en la hoja de datos*/

    if (data[1] & 0x80) /*!< Comprueba si la temperatura es negativa*/
        dht_read_data.temperature *= -1; /*!< Convierte la temperatura a negativa*/

    return dht_read_data.error; /*!< Retorna el estado de la lectura*/
}