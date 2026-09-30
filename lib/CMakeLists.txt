add_library(dirtynet INTERFACE)

add_library(dirtyNet::dirtynet ALIAS dirtynet)

target_include_directories(dirtynet
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_compile_features(dirtynet INTERFACE cxx_std_23)
