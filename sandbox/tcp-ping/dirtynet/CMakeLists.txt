add_subdirectory(server)
add_subdirectory(client)

add_custom_target(sandbox_tcp_ping_dirtynet)
add_dependencies(sandbox_tcp_ping_dirtynet
    sandbox_tcp_ping_dirtynet_server
    sandbox_tcp_ping_dirtynet_client
)
