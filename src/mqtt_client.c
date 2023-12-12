/**
 * @file mqtt_client
 * @brief Implementación de funciones para el manejo de un cliente MQTT utilizando lwip para una raspberry pi pico W
 * @version 0.1
 * @date 2023-10-20
 * @authors Yonathan Lopez Mejía, Sebastian Larrea Henao
*/

#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "mqtt_client.h"
#include <string.h>
#include "app_types.h"

uint8_t inpub_id;

bool init_wifi(void)
{
    /**
     * @brief Función para inicializar el wifi
     * @return true: si se inicializó correctamente, false: si ocurrió un error
    */
    if(cyw43_arch_init_with_country(CYW43_COUNTRY_COLOMBIA))
        return false;

    cyw43_arch_enable_sta_mode();

    if(cyw43_arch_wifi_connect_timeout_ms(SSID, PASS, CYW43_AUTH_WPA2_MIXED_PSK, 10000))
        return false;

    return true;
}

bool init_mqtt(mqtt_client_t **client, struct mqtt_connect_client_info_t *ci, const uint8_t *ip_server)
{   
    /**
     * @brief Función para inicializar el cliente MQTT
     * @param client puntero al puntero de cliente
     * @param ci estructura con la información del cliente
     * @param ip_server ip del servidor MQTT en formato string xxx.xxx.xxx.xxx
     * @return true: si se inicializó correctamente, false: si ocurrió un error
    */
    ip_addr_t broker_ip;
    err_t err;

    if(!init_wifi()) {
        printf("Ocurrió un error iniciando el wifi\n");
        cyw43_deinit(&cyw43_state);
        return false;
    }

    *client = mqtt_client_new();
    
    if(!*client) {
        printf("Ocurrió un error creando el cliente mqtt\n");
        return false;
    }
    
    ipaddr_aton(ip_server, &broker_ip);
    err = mqtt_client_connect(*client, &broker_ip, MQTT_PORT, mqtt_connection_cb, 0, ci);
    
    return true;
    
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
    /**
     * @brief Callback para cuando ocurre una conexión, define las funciones de callback para cuando se recibe un mensaje
     * @param client Cliente MQTT
     * @param arg Argumento para la función de callback
     * @param status Estado de la conexión
     * @return true: si se inicializó correctamente, false: si ocurrió un error
    */
    if(status == MQTT_CONNECT_ACCEPTED) {
        printf("Conexión aceptada\n");
        mqtt_set_inpub_callback(client, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, arg);
    
    } else {
        printf("\nConexión rechazada\n");
    }
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, uint32_t tot_len)
{
    /**
     * @brief Callback para cuando se recibe un mensaje
     * @param arg Argumento para la función de callback
     * @param topic Tema del mensaje
     * @param tot_len Longitud total del mensaje
    */
    if(strcmp(topic, TOPIC_IN) == 0) {
        inpub_id = 0;                       /*<! Si es el tema por el que vienen los datos de ordenes se indica con un cero*/
    }
    else if(strcmp(topic, TOPIC_PUB) == 0) {
        inpub_id = 1;                       /*<! Si lo que se recibe es una respuesta luego de publicar */
    }
}

static void mqtt_incoming_data_cb(void *arg, const uint8_t *data, uint16_t len, uint8_t flags)
{
    /**
     * @brief Callback para cuando se recibe un mensaje
     * @param arg Argumento para la función de callback
     * @param data Datos recibidos
     * @param len Longitud de los datos recibidos
     * @param flags Bandera que indica si es el último paquete de datos
    */

    uint8_t msg[len + 1];
    if(flags && MQTT_DATA_FLAG_LAST) {
        //Se recibió el último paquete de datos
        //Dependiendo del tema que envió el mensaje se toman decisiones
        strncpy(msg, data, len);
        msg[len] = '\0';
        switch (inpub_id) {
        case 0:
            green_sense_flags.start_water_plant = true;
            break;
        default:
            break;
        }
    }
}


bool subscribe_topic(mqtt_client_t **client, const uint8_t *subtopic)
{
    /**
     * @brief Función para suscribir un cliente a un tema
     * @param client puntero al puntero de cliente
     * @param subtopic tema al cual se va a suscribir
    */
    err_t err;
    err = mqtt_subscribe(*client, subtopic, 1, NULL, NULL);

    return err == ERR_OK;
}

bool publish(mqtt_client_t *client, void *arg, const uint8_t *topic, 
            const uint8_t *payload, const uint8_t qos, const uint8_t retain)
{
    /**
     * @brief Publica un mensaje en el broker MQTT
     * @param client Cliente MQTT
     * @param arg Argumento para la función de callback
     * @param topic Tema al que se publicará el mensaje
     * @param payload Mensaje a publicar
     * @param qos Calidad de servicio, 0,1,2
     * @param retain Indica si el mensaje se debe guardar en el broker
     * @return true: publicado correctamente si no, false
    */
    err_t err;
    err = mqtt_publish(client, topic, payload, strlen(payload), qos, retain, mqtt_pub_request_cb, arg);
    
    return err == ERR_OK;
}

static void mqtt_pub_request_cb(void *arg, err_t result)
{
    /**
     *  @brief Callback para cuando ocurre una publicación
     *  @param arg argumento de la función callback
     *  @param result información del resultado de la publicación
    */
    if(result != ERR_OK) {
        printf("Publicación fallida\n");
    }
}