cmake_minimum_required(VERSION 3.10)
project(QuickDrawPride)

# List all the source files needed to compile the app
set(SOURCES
    main.cpp
    selector.cpp
    qd_render.cpp
    bitmaps.cpp
    flags.cpp
    graphics.cpp
    crabs.cpp
    flagdefs.cpp
    flagbrzl.cpp
)

# Create a native classic Mac app.
# TYPE and CREATOR are classic Mac 4-character codes
add_application(QuickDrawPride TYPE "APPL" CREATOR "QDPR" ${SOURCES})
