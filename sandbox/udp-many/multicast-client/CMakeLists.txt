add_executable(sandbox_udp_many_multicast_client
    main.cc
)

set_target_properties(sandbox_udp_many_multicast_client
    PROPERTIES
        OUTPUT_NAME multicast-client
)

target_compile_features(sandbox_udp_many_multicast_client
    PRIVATE
        cxx_std_23
)
