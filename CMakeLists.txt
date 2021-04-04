# What CMake to start at
cmake_minimum_required(VERSION 3.12)

# Include the subsidiary .cmake file to get the SDK
include(pico_sdk_import.cmake)

# Set the name and version of the project
project(PicoRotaryEncoder VERSION 1.0.0)

# Link the Project to a source file (step 4.6)
add_executable(PicoRotaryEncoder source.c)

# Link the Project to an extra library (pico_stdlib)
target_link_libraries(PicoRotaryEncoder hardware_i2c pico_stdlib) #pico_enable_stdio_usb)

# Initialise the SDK
pico_sdk_init()

# Enable USB, UART output => USB and UART output or usb 1, uart 0 for usb output
# pico_enable_stdio_uart(PicoRotaryEncoder 1)
pico_enable_stdio_usb(PicoRotaryEncoder 1)
pico_enable_stdio_uart(PicoRotaryEncoder 0)


# Enable extra outputs (SWD?)
pico_add_extra_outputs(PicoRotaryEncoder)
