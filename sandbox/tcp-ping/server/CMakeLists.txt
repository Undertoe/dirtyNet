add_executable(sandbox_tcp_ping_server
    main.cpp
)

target_compile_features(sandbox_tcp_ping_server
    PRIVATE
        cxx_std_23
)
