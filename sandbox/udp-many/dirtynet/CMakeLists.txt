add_subdirectory(host)
add_subdirectory(unicast-client)
add_subdirectory(multicast-client)

add_custom_target(sandbox_udp_many_dirtynet)
add_dependencies(sandbox_udp_many_dirtynet
    sandbox_udp_many_dirtynet_host
    sandbox_udp_many_dirtynet_unicast_client
    sandbox_udp_many_dirtynet_multicast_client
)
