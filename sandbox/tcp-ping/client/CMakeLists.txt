add_executable(sandbox_tcp_ping_client
    main.cc
)

target_compile_features(sandbox_tcp_ping_client
    PRIVATE
        cxx_std_23
)
