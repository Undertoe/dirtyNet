add_executable(sandbox_tcp_packet_dirtynet_client
    main.cc
)

target_link_libraries(sandbox_tcp_packet_dirtynet_client
    PRIVATE
        sandbox_tcp_packet_dirtynet_shared
)
