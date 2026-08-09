add_executable(sandbox_tcp_ping_dirtynet_server
    main.cc
)

target_link_libraries(sandbox_tcp_ping_dirtynet_server
    PRIVATE
        dirtyNet::dirtynet
)
