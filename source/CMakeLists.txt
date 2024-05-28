cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(PICO_BOARD pico_w)
set(PICO_PLATFORM rp2040)
set(PICO_COMPILER pico_arm_gcc)
set(PROJECT_NAME picow)

set(SRCS_DIR srcs)
set(INCS_DIR incs)

set(WIFI_SSID $ENV{PICO_WIFI_SSID})
set(WIFI_PASS $ENV{PICO_WIFI_PASS})

include_directories(${INCS_DIR})
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(${PROJECT_NAME} C CXX ASM)

add_compile_options(-Wall
   -Wextra
   -DWIFI_SSID="${WIFI_SSID}"
   -DWIFI_PASS="${WIFI_PASS}"
   -g3
)

pico_sdk_init()

add_executable(${PROJECT_NAME}
   ${SRCS_DIR}/main.c
   ${SRCS_DIR}/core/array.c
   ${SRCS_DIR}/core/stringf.c
   ${SRCS_DIR}/core/debug.c
   ${SRCS_DIR}/core/time.c
   ${SRCS_DIR}/routine/state.c
   ${SRCS_DIR}/routine/loop.c
   ${SRCS_DIR}/services/tcp_client.c
   ${SRCS_DIR}/services/wifi_client.c
   )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR} )

target_link_libraries(${PROJECT_NAME}
   pico_stdlib
   pico_malloc
   pico_multicore
   pico_cyw43_arch_none
   pico_cyw43_arch_lwip_threadsafe_background
   )

pico_add_extra_outputs(${PROJECT_NAME})
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
