add_executable(sandbox_tcp_ping_posix_server
    main.cc
)

target_compile_features(sandbox_tcp_ping_posix_server
    PRIVATE
        cxx_std_23
)
