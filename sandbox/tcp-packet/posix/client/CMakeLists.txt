add_executable(sandbox_tcp_packet_posix_client
    main.cc
)

target_compile_features(sandbox_tcp_packet_posix_client
    PRIVATE
        cxx_std_23
)

target_link_libraries(sandbox_tcp_packet_posix_client
    PRIVATE
        sandbox_tcp_packet_posix_shared
)
