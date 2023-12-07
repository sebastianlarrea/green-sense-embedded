#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H
    #include "pico/cyw43_arch.h"
    #include "lwip/apps/mqtt.h"
    #include "lwip/dns.h"
    #define SSID "green-sense"
    #define PASS "holahola"
    #define IO_USERNAME  "green-sense-user"
    #define IO_KEY       "greensense123"
    #define TOPIC_TEST   "CerealKilleer/feeds/otras-pruebitas"
    #define TOPIC_PUB    "hello"
    #define MQTT_SEVER_IP "172.20.10.4"
    
    
    bool init_wifi(void);
    bool init_mqtt(mqtt_client_t **, struct mqtt_connect_client_info_t *, const uint8_t *);
    static void mqtt_connection_cb(mqtt_client_t *, void *, mqtt_connection_status_t);
    static void mqtt_incoming_publish_cb(void *, const char *, uint32_t);
    static void mqtt_incoming_data_cb(void *, const uint8_t *, uint16_t, uint8_t);
    static void mqtt_pub_request_cb(void *, err_t);
    static void toogle(const uint8_t);
    bool subscribe_topic(mqtt_client_t **, const uint8_t *);
    bool publish(mqtt_client_t *, void *, const uint8_t *, 
                const uint8_t *, const uint8_t, const uint8_t);

#endif // MQTT_CLIENT_H