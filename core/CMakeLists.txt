

file(GLOB SRC "*.cc")

add_library(${LIB_NAME} SHARED 
    ${SRC})