add_executable(sandbox_udp_ping_client
    main.cpp
)

target_compile_features(sandbox_udp_ping_client
    PRIVATE
        cxx_std_23
)
