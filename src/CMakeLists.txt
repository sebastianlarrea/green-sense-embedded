add_library(mqtt_client_lib STATIC mqtt_client.c)
add_library(dht22_lib STATIC dht22.c)
add_library(sen0114_lib STATIC sen0114.c)
add_library(pump_level_lib STATIC pump_level.c)
add_library(app STATIC app.c)

target_include_directories(mqtt_client_lib PUBLIC ../include/)
target_include_directories(dht22_lib PUBLIC ../include/)
target_include_directories(sen0114_lib PUBLIC ../include/)
target_include_directories(pump_level_lib PUBLIC ../include/)
target_include_directories(app PUBLIC ../include/)

target_link_libraries(app
                      pico_stdlib
                      mqtt_client_lib
                      sen0114_lib
                      dht22_lib
                      pump_level_lib
                      hardware_sync)

target_link_libraries(mqtt_client_lib
                      pico_stdlib
                      pico_cyw43_arch_lwip_threadsafe_background
                      pico_cyw43_arch_none
                      pico_lwip_mqtt)

target_link_libraries(dht22_lib
                    pico_stdlib
                    hardware_pio
                    hardware_gpio)

target_link_libraries(sen0114_lib
                    pico_stdlib
                    hardware_gpio
                    hardware_adc)

target_link_libraries(pump_level_lib
                    pico_stdlib
                    hardware_gpio)


pico_generate_pio_header(dht22_lib ${CMAKE_CURRENT_LIST_DIR}/dht22.pio)