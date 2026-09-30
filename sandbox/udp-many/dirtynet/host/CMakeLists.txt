add_executable(sandbox_udp_many_dirtynet_host
    main.cc
)

target_link_libraries(sandbox_udp_many_dirtynet_host
    PRIVATE
        dirtyNet::dirtynet
)
