add_executable(sandbox_tcp_packet_dirtynet_server
    main.cc
)

target_link_libraries(sandbox_tcp_packet_dirtynet_server
    PRIVATE
        sandbox_tcp_packet_dirtynet_shared
)
