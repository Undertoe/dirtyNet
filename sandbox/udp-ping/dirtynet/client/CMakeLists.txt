add_executable(sandbox_udp_ping_dirtynet_client
    main.cc
)

target_link_libraries(sandbox_udp_ping_dirtynet_client
    PRIVATE
        dirtyNet::dirtynet
)
