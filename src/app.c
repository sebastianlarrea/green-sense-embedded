/**
 * @file app.c
 * @brief Archivo app del programa que se encarga de inicializar los periféricos, el cliente MQTT enviar los datos de los sensores
 * @authors Sebastian Larrea Henao, Yonathan Lopez Mejia
*/
#include "app.h"
#include "app_types.h"

#define PUMP_LEVEL_GPIO 0 /*!< GPIO para el sensor de nivel de agua*/
#define PIO_W 0 /*!< Código del PIO para el DHT22*/
#define PIO_GPIO 16 /*!< GPIO para el DHT22*/
#define SEN0114_GPIO 26 /*!< GPIO para el sensor de humedad del suelo*/
#define SEN0114_CHAN_SEL 0 /*!< Canal del ADC para el sensor de humedad del suelo*/
#define RELAY_GPIO 2 /*!< GPIO para el relay*/ 
#define WATER_STOP_TIME_MS (1000 * 30) /*!< Tiempo de espera para apagar la bomba de agua*/
#define SEND_DATA_TIME_MS (1000 * 4) /*!< Tiempo de espera para enviar los datos de los sensores*/
#define START_VALVE false /*!< Valor para encender la válvula*/
#define STOP_VALVE true /*!< Valor para apagar la válvula*/
volatile flags_t green_sense_flags; /*!< Variable para almacenar las flags */

void app_main(void)
{
    /**
     * @brief Función principal del programa
     * @note Se encarga de inicializar los periféricos y configurar el RTC
    */
    mqtt_connect_t mqtt_connect;
    struct repeating_timer timer;
    app_init_mqtt(&mqtt_connect);
    dht22_init(PIO_W, PIO_GPIO);
    sen0114_init(SEN0114_GPIO, SEN0114_CHAN_SEL);
    pump_level_init(PUMP_LEVEL_GPIO);
    gpio_set_irq_enabled_with_callback(PUMP_LEVEL_GPIO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, gpios_cb);
    add_repeating_timer_ms(SEND_DATA_TIME_MS, repeating_timer_cb, NULL, &timer);
    app_init_relay();

    while (true) {
        while (green_sense_flags.flags) {
            if (green_sense_flags.send_sensors) {
                green_sense_flags.send_sensors = false;
                app_send_sensors(&mqtt_connect);             
            }

            if (green_sense_flags.start_water_plant) {
                green_sense_flags.start_water_plant = false;
                gpio_put(RELAY_GPIO, START_VALVE); // Enciende la bomba de agua
                add_alarm_in_ms(WATER_STOP_TIME_MS, stop_water_plant_cb, NULL, false);
            }

            if (green_sense_flags.stop_water_plant) {
                green_sense_flags.stop_water_plant = false;
                gpio_put(RELAY_GPIO, STOP_VALVE); // Apaga la bomba de agua
            }
        }
        __wfi();
    }
}

static void app_init_relay(void)
{
    /**
     * @brief Función para inicializar el relay
    */
    gpio_init(RELAY_GPIO);
    gpio_set_dir(RELAY_GPIO, GPIO_OUT);
    gpio_put(RELAY_GPIO, 1); // Inicialmente apagado
}

int64_t stop_water_plant_cb(alarm_id_t id, void *user_data) {
    /**
     * @brief Callback para apagar la bomba de agua
     * @param [in] id ID de la alarma
     * @param [in] user_data Datos del usuario
     * @return 0
    */
    green_sense_flags.stop_water_plant = true;
    return 0;
}

static void app_send_sensors(mqtt_connect_t *mqtt_connect)
{
    /**
     * @brief Función para enviar los datos de los sensores al servidor MQTT
     * @param [in] mqtt_connect Puntero a la estructura mqtt_connect_t donde se almacenó el cliente MQTT
    */
    static uint32_t pub_id = 0;
    uint8_t data[100];
    dht22_read();
    sprintf(data, "%d,%.2f,%.2f,%.2f,%d", 
            pub_id++, dht_read_data.temperature, dht_read_data.humidity, sen0114_read_humidity(), get_pump_level(PUMP_LEVEL_GPIO));

    publish((mqtt_connect->client), NULL, TOPIC_PUB, data, 2, 0);
    dht22_restart();
}

static void app_init_mqtt(mqtt_connect_t *mqtt_connect)
{
    /**
     * @brief Inicializa el cliente MQTT
     * @param [in] mqtt_connect Puntero a la estructura mqtt_connect_t donde se almacenará el cliente MQTT
     * */ 

    memset(&(mqtt_connect->ci), 0, sizeof(mqtt_connect->ci));
    mqtt_connect->ci.client_id = "pico_w";
    mqtt_connect->ci.keep_alive = 0;
    mqtt_connect->ci.client_user = IO_USERNAME;
    mqtt_connect->ci.client_pass = IO_KEY;

    while (!init_mqtt(&(mqtt_connect->client), &(mqtt_connect->ci), MQTT_SEVER_IP)) {
        printf("Ocurrió un error iniciando el cliente MQTT. Reintentado...\n");
    }
    
    while(!subscribe_topic(&(mqtt_connect->client), TOPIC_PUB)) {
        printf("No se pudo suscribir al tema %s\n", TOPIC_PUB);
    }

    while(!subscribe_topic(&(mqtt_connect->client), TOPIC_IN)) {
        printf("No se pudo suscribir al tema %s\n", TOPIC_IN);
    }

    printf("Tema suscrito\n");
}

static void gpios_cb(uint gpio, uint32_t event) 
{
    /**
     * @brief Callback para los GPIOs
     * @param [in] gpio GPIO que generó la interrupción
     * @param [in] event Evento que generó la interrupción
    */
    switch (gpio) {
    case PUMP_LEVEL_GPIO:
        printf("Nivel de agua: %d\n", gpio_get(gpio));
        break;
    default:
        break;
    }
}


static bool repeating_timer_cb(struct repeating_timer *t)
{
    /**
     * @brief Callback para el timer que se encarga de enviar los datos de los sensores
     * @param [in] t Puntero al timer
     * @return true: si se inicializó correctamente, false: si ocurrió un error
    */
    green_sense_flags.send_sensors = true;
    return true;
}