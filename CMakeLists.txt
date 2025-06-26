# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.6)

# Set board type because we are building for PicoW
set(PICO_BOARD pico_w)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(pico-modbus C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

#add_compile_definitions(MQTTCLIENT_QOS2=1)

add_compile_options(-Wall
        -Wno-format
        -Wno-unused-function
        -Wno-maybe-uninitialized
)
file(GLOB MQTT_SOURCES "paho.mqtt.embedded-c/MQTTPacket/src/*.c")

# Tell CMake where to find the executable source files
add_executable(${PROJECT_NAME}
        main.cpp
        DoorController.cpp
        Motor.cpp
        Encoder.cpp
        Buttons.cpp
        LedController.cpp
        uart/PicoUart.cpp
        uart/PicoUart.h
        mqtt/Countdown.cpp
        mqtt/Countdown.h
        mqtt/IPStack.cpp
        mqtt/IPStack.h
        mqtt/lwipopts.h
        ${MQTT_SOURCES}
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Include directories for MQTT and UART (poistettu modbus ja display)
target_include_directories(${PROJECT_NAME} PRIVATE
        mqtt
        uart
        paho.mqtt.embedded-c/MQTTPacket/src
        paho.mqtt.embedded-c/MQTTClient/src
)

# Link required libraries: pico_stdlib, hardware GPIO/I2C, and WiFi/TCP/IP libraries for Pico W
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        hardware_i2c
        hardware_gpio
        hardware_pwm
        pico_cyw43_arch_lwip_poll
        pico_cyw43_arch
        pico_lwip
)

# Enable USB stdio output (disable UART stdio if not needed)
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
