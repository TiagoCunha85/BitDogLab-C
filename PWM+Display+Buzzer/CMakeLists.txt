# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 1.5.1)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.13)

# Set the C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Enable exporting compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Specify the board type (Raspberry Pi Pico W by default)
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Include the Raspberry Pi Pico SDK import script
include(pico_sdk_import.cmake)

# Define the project name and languages
project(BitDogLab C CXX ASM)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Add the executable with the source files
add_executable(BitDogLab 
    BitDogLab.c 
    ssd1306_i2c.c 
    play_audio.c
)

# Set the program name and version
pico_set_program_name(BitDogLab "BitDogLab")
pico_set_program_version(BitDogLab "0.1")

# Enable or disable output over UART/USB
pico_enable_stdio_uart(BitDogLab 0)  # Disable UART
pico_enable_stdio_usb(BitDogLab 1)   # Enable USB

# Link the standard library, hardware libraries, and SSD1306 library
target_link_libraries(BitDogLab
    pico_stdlib
    hardware_i2c
    hardware_pwm
    hardware_adc
)

# Specify the include directories for the project
target_include_directories(BitDogLab PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/inc        # Diretório de includes
)

# Add the SSD1306 specific include path
target_include_directories(BitDogLab PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/ssd1306    # Diretório de cabeçalhos do SSD1306
)

# Add extra outputs for the project (e.g., .bin, .uf2 files)
pico_add_extra_outputs(BitDogLab)
