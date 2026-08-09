add_executable(sandbox_tcp_multi_client
    main.cc
)

target_compile_features(sandbox_tcp_multi_client
    PRIVATE
        cxx_std_23
)
