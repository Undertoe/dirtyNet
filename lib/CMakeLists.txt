add_library(dirtynet
    dirtynet.cpp
)

add_library(dirtyNet::dirtynet ALIAS dirtynet)

target_include_directories(dirtynet
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(dirtynet PUBLIC cxx_std_23)
