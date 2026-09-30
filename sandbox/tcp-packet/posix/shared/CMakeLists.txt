add_library(sandbox_tcp_packet_posix_shared SHARED
    shared.cc
)

target_compile_features(sandbox_tcp_packet_posix_shared
    PUBLIC
        cxx_std_23
)

target_include_directories(sandbox_tcp_packet_posix_shared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
