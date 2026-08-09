add_executable(sandbox_udp_many_posix_unicast_client
    main.cc
)

target_compile_features(sandbox_udp_many_posix_unicast_client
    PRIVATE
        cxx_std_23
)
