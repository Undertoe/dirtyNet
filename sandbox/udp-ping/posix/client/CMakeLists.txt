add_executable(sandbox_udp_ping_posix_client
    main.cc
)

target_compile_features(sandbox_udp_ping_posix_client
    PRIVATE
        cxx_std_23
)
