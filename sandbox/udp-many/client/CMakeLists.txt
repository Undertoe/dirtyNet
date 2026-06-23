add_executable(sandbox_udp_many_client
    main.cc
)

target_compile_features(sandbox_udp_many_client
    PRIVATE
        cxx_std_23
)
