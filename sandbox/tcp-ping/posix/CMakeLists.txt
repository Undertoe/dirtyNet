add_subdirectory(server)
add_subdirectory(client)

add_custom_target(sandbox_tcp_ping_posix)
add_dependencies(sandbox_tcp_ping_posix
    sandbox_tcp_ping_posix_server
    sandbox_tcp_ping_posix_client
)
