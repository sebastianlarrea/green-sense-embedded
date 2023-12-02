#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H
    #include "pico/cyw43_arch.h"
    #include "lwip/apps/mqtt.h"
    #include "lwip/dns.h"
    #define SSID "Celeste :3"
    #define PASS "Celeste246-"
    #define IO_USERNAME  "CerealKilleer"
    #define IO_KEY       "aio_ippW76eXuAO7N5HluOhpPx8SVHeg"
    #define TOPIC_TEST   "CerealKilleer/feeds/otras-pruebitas"
    #define TOPIC_PUB    "CerealKilleer/feeds/publicar"
    #define MQTT_SEVER_IP "52.54.110.50"
    
    
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