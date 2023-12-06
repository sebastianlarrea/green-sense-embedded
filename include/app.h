#ifndef __APP_H__
#define __APP_H__
    #include <stdio.h>
    #include "pico/stdlib.h"
    #include "pico/util/datetime.h"
    #include "mqtt_client.h"
    #include "hardware/sync.h"
    #include "ntp_client.h"
    #include "dht22.h"
    #include "sen0114.h"
    #include "pump_level.h"
    #include "ds3231.h"
    typedef struct {
        struct mqtt_connect_client_info_t ci;
        mqtt_client_t *client;
    }mqtt_connect_t;

    static void app_init_mqtt(mqtt_connect_t *mqtt_connect);
    static void gpios_cb(uint, uint32_t);
    static void app_startup_rtc_config(void);
    void app_main(void);
#endif // __APP_H__
