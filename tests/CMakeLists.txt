
file(GLOB SRC "*.cc")

include_directories(../core)

add_executable(${TEST_NAME} ${SRC})


target_link_libraries(${TEST_NAME} ${LIB_NAME} pthread Catch2::Catch2WithMain)




add_test(NAME ${TEST_NAME}
        COMMAND ${TEST_NAME})