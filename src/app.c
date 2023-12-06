#include "app.h"

#define DS3231_SDA 14
#define DS3231_SCL 15
#define DS3231_ALARM_GPIO 13
#define DS3231_I2C_CHANNEL 1
#define PUMP_LEVEL_GPIO 4
#define PIO_W 0
#define PIO_GPIO 16
#define SEN0114_GPIO 26
#define SEN0114_CHAN_SEL 0


void app_main(void)
{
    /**
     * @brief Función principal del programa
     * @note Se encarga de inicializar los periféricos y configurar el RTC
    */
    mqtt_connect_t mqtt_connect;
    app_init_mqtt(&mqtt_connect);
    dht22_init(PIO_W, PIO_GPIO);
    sen0114_init(SEN0114_GPIO, SEN0114_CHAN_SEL);
    ds3231_init(DS3231_SDA, DS3231_SCL, DS3231_ALARM_GPIO, DS3231_I2C_CHANNEL);
    app_startup_rtc_config();
    pump_level_init(PUMP_LEVEL_GPIO);
    gpio_set_irq_enabled_with_callback(PUMP_LEVEL_GPIO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, gpios_cb);
    gpio_set_irq_enabled_with_callback(DS3231_ALARM_GPIO, GPIO_IRQ_EDGE_FALL, true, gpios_cb);

    datetime_t alarm = {
        .day = utc->tm_mday,
        .month = utc->tm_mon + 1,
        .year = utc->tm_year + 1900,
        .hour = utc->tm_hour,
        .min = utc->tm_min,
        .sec = utc->tm_sec + 10,
        .dotw = utc->tm_wday,
    };
    //datetime_t time;
    //ds3231_set_alarm1(&alarm);
    

    while (true) {
        ds3231_clear_alarm();
        /*
        if(!dht22_read()) {
            printf("Temperatura: %.2f\n", dht_read_data.temperature);
            printf("Humedad: %.2f\n", dht_read_data.humidity);
        } else {
            printf("Error leyendo el sensor\n");
        }
        printf("% Humedad del suelo: %.2f\n", sen0114_read_humidity());
        printf("Nivel de agua: %s\n", get_pump_level(PUMP_LEVEL_GPIO) ? "Normal" : "Bajo");
        busy_wait_ms(1000);

        dht22_restart();
        */
        __wfi();
    }
}

static void app_init_mqtt(mqtt_connect_t *mqtt_connect)
{
    /**
     * @brief Inicializa el cliente MQTT
     * @param mqtt_connect Puntero a la estructura mqtt_connect_t donde se almacenará el cliente MQTT
     * */ 

    memset(&(mqtt_connect->ci), 0, sizeof(mqtt_connect->ci));
    mqtt_connect->ci.client_id = "pico_w";
    mqtt_connect->ci.keep_alive = 0;
    mqtt_connect->ci.client_user = IO_USERNAME;
    mqtt_connect->ci.client_pass = IO_KEY;

    while (!init_mqtt(&(mqtt_connect->client), &(mqtt_connect->ci), MQTT_SEVER_IP)) {
        printf("Ocurrió un error iniciando el cliente MQTT. Reintentado...\n");
    }
    
    while(!subscribe_topic(&(mqtt_connect->client), TOPIC_TEST)) {
        printf("No se pudo suscribir al tema\n");
    }

    while(!subscribe_topic(&(mqtt_connect->client), TOPIC_PUB)) {
        printf("No se pudo suscribir al tema\n");
    }

    printf("Tema suscrito\n");
    if(publish((mqtt_connect->client), NULL, TOPIC_PUB, "Hola desde Pico :3", 2, 0)) {
        printf("Publicado correctamente\n");
    }
}

static void gpios_cb(uint gpio, uint32_t event) 
{
    switch (gpio) {
    case DS3231_ALARM_GPIO:
        printf("Alarma del DS3231\n");
        ds3231_clear_alarm();
        break;
    case PUMP_LEVEL_GPIO:
        printf("Nivel de agua: %d\n", gpio_get(gpio));
        break;
    default:
        break;
    }
}

static void app_startup_rtc_config(void)
{
    /**
     * @brief Configura el RTC con la fecha y hora obtenida del servidor NTP
    */

    if(ntp_get_time()) {
        printf("Fecha y hora: %s\n", asctime(utc));
    } else {
        printf("Error obteniendo la fecha y hora\n");
        exit(EXIT_FAILURE);
    }

    datetime_t time = {
        .day = utc->tm_mday,
        .month = utc->tm_mon + 1,
        .year = utc->tm_year + 1900,
        .hour = utc->tm_hour,
        .min = utc->tm_min,
        .sec = utc->tm_sec,
        .dotw = utc->tm_wday,
    };
    ds3231_set_time(&time);
    ds3231_clear_alarm();
}