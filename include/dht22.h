#ifndef DHT22_H
#define DHT22_H

typedef struct{
    float temperature; /*!< Temperatura en grados Celsius*/
    float humidity; /*!< Humedad relativa en %*/
    bool error; /*!< Indica si hubo un error en la lectura*/
}dht_data; /*!< Estructura para almacenar los datos del DHT22*/

extern dht_data dht_read_data; /*!< Estructura para almacenar los datos del DHT22*/

#define PIO_CLKDIV 255 /*!< Divisor de reloj del PIO*/
#define START_DELAY 500 /*!< Valor para generar el pulso bajo de 1ms*/

void dht22_init(uint8_t pio_code, uint8_t pin); /*!< Configura el modulo PIO para el DHT22*/
bool dht22_read(void); /*!< Lee los datos del DHT22*/
void dht22_restart(void); /*!< Deshabilita el state machine y libera el PIO*/

#endif /* DHT22_H */