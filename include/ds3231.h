#ifndef __DS3231_H__
#define __DS3231_H__
    #define DS3231_BAUDRATE (400 * 1000)
    #define DS3231_SET_TIME_FIELDS 8
    #define DS3231_ADDR 0x68
    #define BIN_2BCD(bin) (((bin) % 10) | (((bin) / 10) << 4))
    #define BCD_2BIN(bcd) (((bcd) & 0x0F) + (((bcd) >> 4) * 10))

    void ds3231_init(uint8_t, uint8_t, uint8_t, uint8_t); /*<! Inicializa la interfaz i2c*/
    bool ds3231_set_time(const datetime_t *); /*<! Configura la fecha y hora del DS3231*/
    bool ds3231_get_date(datetime_t *); /*<! Obtiene la fecha y hora del DS3231*/
    bool ds3231_set_alarm1(datetime_t *); /*<! Configura la alarma del DS3231*/
    void ds3231_clear_alarm(void); /*<! Limpia la bandera de alarma del DS3231*/
#endif // __DS3231_H__