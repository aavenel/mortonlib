file(GLOB SOURCES
    *.h
)

add_library(mortonlib morton.cpp ${SOURCES})