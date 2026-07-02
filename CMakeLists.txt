cmake_minimum_required(VERSION 3.0)
project(VGAPride)

# List all the source files needed to compile the app
set(SOURCES
    vgapride.cpp
    flags.cpp
    display.cpp
    graphics.cpp
    crabs.cpp
    lz4.c 
)

# Create a PowerPC Mac app with a Console
# TYPE and CREATOR are classic Mac 4-character codes
add_application(VGAPride CONSOLE TYPE "APPL" CREATOR "VGAP" ${SOURCES})