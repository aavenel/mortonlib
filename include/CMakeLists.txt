file(GLOB SOURCES_INCLUDE
    *.h
)

add_library(mortonlib morton.cpp ${SOURCES_INCLUDE})