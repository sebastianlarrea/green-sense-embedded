/**
 * @file app.c
 * @brief Archivo app del programa que se encarga de inicializar los periféricos, el cliente MQTT enviar los datos de los sensores
 * @authors Sebastian Larrea Henao, Yonathan Lopez Mejia
*/
#include "app.h"

#define PUMP_LEVEL_GPIO 0 /*!< GPIO para el sensor de nivel de agua*/
#define PIO_W 0 /*!< Código del PIO para el DHT22*/
#define PIO_GPIO 16 /*!< GPIO para el DHT22*/
#define SEN0114_GPIO 26 /*!< GPIO para el sensor de humedad del suelo*/
#define SEN0114_CHAN_SEL 0 /*!< Canal del ADC para el sensor de humedad del suelo*/
#define RELAY_GPIO 2 /*!< GPIO para el relay*/ 

typedef union {
    uint8_t flags;
    struct {
        bool send_sensors : 1;
        uint8_t : 7;
    };
} flags_t;  /*!<Tipo para las flags*/

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
    add_repeating_timer_ms(4000, repeating_timer_cb, NULL, &timer);
    app_init_relay();

    while (true) {
        while (green_sense_flags.flags) {
            if(green_sense_flags.send_sensors) {
                green_sense_flags.send_sensors = false;
                app_send_sensors(&mqtt_connect);              
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
    gpio_put(RELAY_GPIO, 0); // Inicialmente apagado
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