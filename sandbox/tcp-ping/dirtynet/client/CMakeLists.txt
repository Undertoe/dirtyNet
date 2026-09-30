add_executable(sandbox_tcp_ping_dirtynet_client
    main.cc
)

target_link_libraries(sandbox_tcp_ping_dirtynet_client
    PRIVATE
        dirtyNet::dirtynet
)
