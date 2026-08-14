add_executable(sandbox_udp_many_dirtynet_unicast_client
    main.cc
)

target_link_libraries(sandbox_udp_many_dirtynet_unicast_client
    PRIVATE
        dirtyNet::dirtynet
)
