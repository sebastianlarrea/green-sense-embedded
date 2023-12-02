#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "mqtt_client.h"
#include "hardware/sync.h"
#include "hardware/rtc.h"
#include "ntp_client.h"
#include "dht22.h"
#include "sen0114.h"

void rtc_config(void);
static void alarm_callback(void);

static void alarm_callback(void)
{
    printf("Alarma\n");

}

int main(void)
{
    stdio_init_all();

    struct mqtt_connect_client_info_t ci;
    mqtt_client_t *client;
    memset(&ci, 0, sizeof(ci));
    ci.client_id = "pico_w";
    ci.keep_alive = 0;
    ci.client_user = IO_USERNAME;
    ci.client_pass = IO_KEY;
    
    while (!init_mqtt(&client, &ci, MQTT_SEVER_IP)) {
        printf("Ocurrió un error iniciando el cliente MQTT. Reintentado...\n");
    }
    
    while(!subscribe_topic(&client, TOPIC_TEST)) {
        printf("No se pudo suscribir al tema\n");
    }

    while(!subscribe_topic(&client, TOPIC_PUB)) {
        printf("No se pudo suscribir al tema\n");
    }

    printf("Tema suscrito\n");
    if(publish(client, NULL, TOPIC_PUB, "Hola desde Pico", 2, 0)) {
        printf("Publicado correctamente\n");
    }
    /**
    if(ntp_get_time()) {
        printf("Fecha y hora: %s\n", asctime(utc));
        rtc_config();
    } else {
        printf("Error obteniendo la fecha y hora\n");
        exit(EXIT_FAILURE);
    }
    */

    dht22_init(0, 16);
    sen0114_init(SEN0114_GPIO, SEN0114_CHAN_SEL);
    while(true) {
        if(!dht22_read()) {
            printf("Temperatura: %.2f\n", dht_read_data.temperature);
            printf("Humedad: %.2f\n", dht_read_data.humidity);
        } else {
            printf("Error leyendo el sensor\n");
        }
        printf("% Humedad del suelo: %.2f\n", sen0114_read_humidity());
        busy_wait_ms(2000);
        dht22_restart();
    }
    return 0;
}

void rtc_config(void)
{
    /**
     * @brief Configura el RTC
    */
    datetime_t t = {
        .day = utc->tm_mday,
        .month = utc->tm_mon + 1,
        .year = utc->tm_year + 1900,
        .hour = utc->tm_hour,
        .min = utc->tm_min,
        .sec = utc->tm_sec,
        .dotw = utc->tm_wday,
    };

    datetime_t alarm = {
    .year  = -1,
    .month = -1,
    .day   = -1,
    .dotw  = -1,
    .hour  = -1,
    .min   = -1,
    .sec   = 15
    };

    rtc_init();
    rtc_set_datetime(&t);
    rtc_set_alarm(&alarm, &alarm_callback);
}