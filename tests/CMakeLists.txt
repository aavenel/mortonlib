file(GLOB SOURCES_TESTS
    *.h
)

add_executable(morton_test tests.cpp ${SOURCES_TESTS})
target_link_libraries(morton_test mortonlib)