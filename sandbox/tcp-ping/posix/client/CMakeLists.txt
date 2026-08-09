add_executable(sandbox_tcp_ping_posix_client
    main.cc
)

target_compile_features(sandbox_tcp_ping_posix_client
    PRIVATE
        cxx_std_23
)
