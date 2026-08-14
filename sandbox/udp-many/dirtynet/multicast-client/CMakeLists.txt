add_executable(sandbox_udp_many_dirtynet_multicast_client
    main.cc
)

target_link_libraries(sandbox_udp_many_dirtynet_multicast_client
    PRIVATE
        dirtyNet::dirtynet
)
