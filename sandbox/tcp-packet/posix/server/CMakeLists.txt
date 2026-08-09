add_executable(sandbox_tcp_packet_posix_server
    main.cc
)

target_compile_features(sandbox_tcp_packet_posix_server
    PRIVATE
        cxx_std_23
)

target_link_libraries(sandbox_tcp_packet_posix_server
    PRIVATE
        sandbox_tcp_packet_posix_shared
)
