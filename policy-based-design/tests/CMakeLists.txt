set(PROJECT_TESTS "tests-${PROJECT_ID}")
set(PROJECT_TESTS "tests-${PROJECT_ID}" PARENT_SCOPE)
message(STATUS "PROJECT_TESTS is: " ${PROJECT_TESTS})

####################
# Sources & headers
aux_source_directory(. SRC_LIST)
file(GLOB HEADERS_LIST "*.h" "*.hpp")

add_executable(${PROJECT_TESTS} ${SRC_LIST} ${HEADERS_LIST})

target_link_libraries(${PROJECT_TESTS} PRIVATE Catch2::Catch2WithMain ${PROJECT_LIB})

# add_test(NAME ${PROJECT_TESTS}
#          COMMAND ${PROJECT_TESTS})

catch_discover_tests(${PROJECT_TESTS})