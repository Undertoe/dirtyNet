add_executable(sandbox_udp_many_unicast_client
    main.cc
)

set_target_properties(sandbox_udp_many_unicast_client
    PROPERTIES
        OUTPUT_NAME unicast-client
)

target_compile_features(sandbox_udp_many_unicast_client
    PRIVATE
        cxx_std_23
)
