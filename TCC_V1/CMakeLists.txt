# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
include_directories( ${CMAKE_SOURCE_DIR}/lib ) # Inclui os files .h na pasta lib
project(TCC_V1 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_subdirectory(lib/FatFs_SPI)
add_executable(TCC_V1
        TCC_V1.c
        hw_config.c
        lib/medicao.c
        lib/oled_display.c
        lib/sd_logger.c
        lib/ssd1306.c
        lib/ntp_sync.c
        lib/base44_client.c
)

pico_set_program_name(TCC_V1 "TCC_V1")
pico_set_program_version(TCC_V1 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(TCC_V1 0)
pico_enable_stdio_usb(TCC_V1 1)

target_link_libraries(TCC_V1
        pico_stdlib
        FatFs_SPI
        hardware_adc
        hardware_clocks
        hardware_i2c
        hardware_pwm
        pico_aon_timer
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_sntp
        pico_lwip_mbedtls
        pico_mbedtls)

target_compile_definitions(TCC_V1 PRIVATE
        MQTT_CERT_INC=1   # liga LWIP_ALTCP_TLS em lwipopts.h
)
# Add the standard include files to the build
target_include_directories(TCC_V1 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries


pico_add_extra_outputs(TCC_V1)