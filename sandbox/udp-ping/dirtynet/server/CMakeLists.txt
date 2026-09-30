add_executable(sandbox_udp_ping_dirtynet_server
    main.cc
)

target_link_libraries(sandbox_udp_ping_dirtynet_server
    PRIVATE
        dirtyNet::dirtynet
)
