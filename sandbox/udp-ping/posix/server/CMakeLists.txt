add_executable(sandbox_udp_ping_posix_server
    main.cc
)

target_compile_features(sandbox_udp_ping_posix_server
    PRIVATE
        cxx_std_23
)
