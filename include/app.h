#ifndef __APP_H__
#define __APP_H__
    #include <stdio.h>
    #include "pico/stdlib.h"
    #include "pico/time.h"
    #include "mqtt_client.h"
    #include "hardware/sync.h"
    #include "dht22.h"
    #include "sen0114.h"
    #include "pump_level.h"
    #include <string.h>
    typedef struct {
        struct mqtt_connect_client_info_t ci;
        mqtt_client_t *client;
    }mqtt_connect_t; /*!< Estructura para almacenar los datos de la conexión MQTT*/

    static void app_init_mqtt(mqtt_connect_t *mqtt_connect);
    static void gpios_cb(uint, uint32_t);
    void app_main(void);
    static void app_send_sensors(mqtt_connect_t *);
    static bool repeating_timer_cb(struct repeating_timer *);
    static void app_init_relay(void);
    int64_t stop_water_plant_cb(alarm_id_t, void *);
#endif // __APP_H__
