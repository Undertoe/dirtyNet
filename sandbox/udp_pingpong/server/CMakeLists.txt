add_executable(sandbox_udp_pingpong_server
    main.cpp
)

target_compile_features(sandbox_udp_pingpong_server
    PRIVATE
        cxx_std_23
)
