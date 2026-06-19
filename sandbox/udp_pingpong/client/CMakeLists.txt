add_executable(sandbox_udp_pingpong_client
    main.cpp
)

target_compile_features(sandbox_udp_pingpong_client
    PRIVATE
        cxx_std_23
)
