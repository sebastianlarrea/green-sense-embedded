#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "hardware/i2c.h"
#include "ds3231.h"

i2c_inst_t *i2cx; /*<! Instancia del i2c*/

void ds3231_init(uint8_t sda_gpio, uint8_t scl_gpio, uint8_t alarm_gpio, uint8_t i2c_channel)
{
    /**
     * @brief Configura el bus I2C para comunicarse con el DS3231
     * @param sda_gpio GPIO del SDA
     * @param scl_gpio GPIO del SCL
    */
    
    i2cx = i2c_channel ? i2c1 : i2c0;
    gpio_init(alarm_gpio);
    gpio_set_dir(alarm_gpio, GPIO_IN);
    gpio_set_function(sda_gpio, GPIO_FUNC_I2C);
    gpio_set_function(scl_gpio, GPIO_FUNC_I2C);
    gpio_pull_up(sda_gpio);
    gpio_pull_up(scl_gpio);
    gpio_pull_up(alarm_gpio);
    i2c_init(i2cx, DS3231_BAUDRATE);
}

bool ds3231_set_time(const datetime_t *new_time)
{
    /**
     * @brief Configura la fecha y hora del DS3231
     * @return true si se configura correctamente, false en caso contrario
    */
    uint8_t *data = (uint8_t *)calloc(DS3231_SET_TIME_FIELDS, sizeof(uint8_t));
    int data_cnt;
    data[1] = BIN_2BCD(new_time->sec);
    data[2] = BIN_2BCD(new_time->min);
    data[3] = BIN_2BCD(new_time->hour);
    data[4] = BIN_2BCD(new_time->dotw);
    data[5] = BIN_2BCD(new_time->day);
    data[6] = BIN_2BCD(new_time->month);
    data[7] = BIN_2BCD(new_time->year % 100); // Solo se envían los últimos dos dígitos del año
    data_cnt = i2c_write_blocking(i2cx, DS3231_ADDR, data, DS3231_SET_TIME_FIELDS, false);
    if (data_cnt != DS3231_SET_TIME_FIELDS) {
        free(data);
        printf("Error configurando la fecha y hora\n");
        return false;
    }
    free(data);
    return true;
}

bool ds3231_get_date(datetime_t *time)
{
    /**
     * @brief Obtiene la fecha y hora del DS3231
     * @param time Puntero a la estructura datetime_t donde se almacenará la fecha y hora
     * @return true si se obtiene correctamente, false en caso contrario
    */
    int data_cnt;
    const uint8_t addr = 0x00;
    uint8_t *data = (uint8_t *)calloc(DS3231_SET_TIME_FIELDS, sizeof(uint8_t));
    i2c_write_blocking(i2cx, DS3231_ADDR, &addr, 1, true);
    data_cnt = i2c_read_blocking(i2cx, DS3231_ADDR, data, DS3231_SET_TIME_FIELDS - 1, false);
    time->sec = BCD_2BIN(data[0]);
    time->min = BCD_2BIN(data[1]);
    time->hour = BCD_2BIN(data[2]);
    time->dotw = BCD_2BIN(data[3]);
    time->day = BCD_2BIN(data[4]);
    time->month = BCD_2BIN(data[5]);
    time->year = BCD_2BIN(data[6]) + 2000; // Se le suman 2000 a los últimos dos dígitos del año

    free(data);

    if (data_cnt != DS3231_SET_TIME_FIELDS - 1) {
        printf("Error obteniendo la fecha y hora\n");
        return false;
    }
    return true;
}

bool ds3231_set_alarm1(datetime_t *time)
{
    /**
     * @brief Configura la alarma 1 del DS3231, esta alarma se repetirá todos los días a la hora especificada
     * @param time Puntero a la estructura datetime_t donde se almacena la informacion de la hora de la alarma
     * @return true si se configura correctamente, false en caso contrario
    */
    
    const uint8_t alarm1_addr = 0x07;
    const uint8_t alarm1_fields = 5;
    uint8_t *data = (uint8_t *)calloc(alarm1_fields, sizeof(uint8_t));
    uint data_cnt;
    data[0] = alarm1_addr;
    data[1] = BIN_2BCD(time->sec) | 0x80;
    data[2] = BIN_2BCD(time->min) | 0x80;
    data[3] = BIN_2BCD(time->hour) | 0x80;
    data[4] = BIN_2BCD(time->dotw) | 0xB0;
    
    data_cnt = i2c_write_blocking(i2cx, DS3231_ADDR, data, alarm1_fields, false);
    if (data_cnt != alarm1_fields) {
        free(data);
        printf("Error escribiendo las horas de activacion de la alarma\n");
        return false;
    }

    data[0] = 0x0E; // Dirección del registro de control de la salida SQW
    data[1] = 0x05; // Se configura la salida SQW para que sea de 1Hz en la alarma 1
    data_cnt = i2c_write_blocking(i2cx, DS3231_ADDR, data, 2, false); // Se activa la salida SQW para que se active cuando se active la alarma
    if (data_cnt != 2) {
        free(data);
        printf("Error escribiendo el registro de control de la salida SQW\n");
        return false;
    }
    printf("Alarma configurada para %02d:%02d:%02d\n", time->hour, time->min, time->sec);
    free(data);
    return true;
}

void ds3231_clear_alarm(void)
{
    /**
     * @brief Limpia la bandera de alarma del DS3231
    */
    const uint8_t data[2] = {0x0F, 0x00};
    i2c_write_blocking(i2cx, DS3231_ADDR, data, 2, false);
}