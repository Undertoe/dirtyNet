add_library(sandbox_tcp_packet_dirtynet_shared SHARED
    shared.cc
)

target_link_libraries(sandbox_tcp_packet_dirtynet_shared
    PUBLIC
        dirtyNet::dirtynet
)

target_include_directories(sandbox_tcp_packet_dirtynet_shared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
