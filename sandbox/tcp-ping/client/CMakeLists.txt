add_executable(sandbox_tcp_ping_client
    main.cpp
)

target_compile_features(sandbox_tcp_ping_client
    PRIVATE
        cxx_std_23
)
