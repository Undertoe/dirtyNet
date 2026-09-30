add_subdirectory(host)
add_subdirectory(unicast-client)
add_subdirectory(multicast-client)

add_custom_target(sandbox_udp_many_posix)
add_dependencies(sandbox_udp_many_posix
    sandbox_udp_many_posix_host
    sandbox_udp_many_posix_unicast_client
    sandbox_udp_many_posix_multicast_client
)
