add_executable(${CMAKE_PROJECT_NAME} 
                ../src/main.c)

target_link_libraries(${CMAKE_PROJECT_NAME}
                      pico_stdlib
                      pico_cyw43_arch_lwip_threadsafe_background
                      pico_cyw43_arch_none
                      pico_lwip_mqtt
                      app)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ../include)

pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)