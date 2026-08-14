add_subdirectory(shared)
add_subdirectory(server)
add_subdirectory(client)

add_custom_target(sandbox_tcp_packet_dirtynet)
add_dependencies(sandbox_tcp_packet_dirtynet
    sandbox_tcp_packet_dirtynet_server
    sandbox_tcp_packet_dirtynet_client
)
