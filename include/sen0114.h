#ifndef __SEN0114_H__
#define __SEN0114_H__


#define SEN0114_MAX 2500

void sen0114_init(const uint8_t, const uint8_t); /*<! Inicializa el modulo*/
static uint16_t sen0114_read(void); /*<! Lee el valor del sensor*/
float sen0114_read_humidity(void); /*<! Lee el valor del sensor y lo convierte a porcentaje de humedad*/

#endif /* __SEN0114_H__ */