add_subdirectory(server)
add_subdirectory(client)

add_custom_target(sandbox_udp_ping_posix)
add_dependencies(sandbox_udp_ping_posix
    sandbox_udp_ping_posix_server
    sandbox_udp_ping_posix_client
)
