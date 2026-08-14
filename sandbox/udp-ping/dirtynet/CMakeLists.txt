add_subdirectory(server)
add_subdirectory(client)

add_custom_target(sandbox_udp_ping_dirtynet)
add_dependencies(sandbox_udp_ping_dirtynet
    sandbox_udp_ping_dirtynet_server
    sandbox_udp_ping_dirtynet_client
)
