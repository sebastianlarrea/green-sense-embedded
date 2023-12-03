add_executable(${CMAKE_PROJECT_NAME} 
                ../src/main.c)

target_link_libraries(${CMAKE_PROJECT_NAME}
                      pico_stdlib
                      mqtt_client_lib
                      ntp_client_lib
                      sen0114_lib
                      dht22_lib
                      pump_level_lib
                      hardware_sync
                      hardware_rtc)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ../include)

pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)