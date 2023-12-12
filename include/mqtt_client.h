#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H
    #include "pico/cyw43_arch.h"
    #include "lwip/apps/mqtt.h"
    #include "lwip/dns.h"
    #define SSID "Celeste :3"
    #define PASS "Celeste246-"
    #define IO_USERNAME  "green-sense-user"
    #define IO_KEY       "greensense123"
    #define TOPIC_IN   "green-sense-active-valve"
    #define TOPIC_PUB    "green-sense-read-sensors"
    #define MQTT_SEVER_IP "192.168.88.180"
    
    
    bool init_wifi(void);
    bool init_mqtt(mqtt_client_t **, struct mqtt_connect_client_info_t *, const uint8_t *);
    static void mqtt_connection_cb(mqtt_client_t *, void *, mqtt_connection_status_t);
    static void mqtt_incoming_publish_cb(void *, const char *, uint32_t);
    static void mqtt_incoming_data_cb(void *, const uint8_t *, uint16_t, uint8_t);
    static void mqtt_pub_request_cb(void *, err_t);
    bool subscribe_topic(mqtt_client_t **, const uint8_t *);
    bool publish(mqtt_client_t *, void *, const uint8_t *, 
                const uint8_t *, const uint8_t, const uint8_t);

#endif // MQTT_CLIENT_H