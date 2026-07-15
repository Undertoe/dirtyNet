add_executable(sandbox_tcp_multi_server
    main.cc
)

target_compile_features(sandbox_tcp_multi_server
    PRIVATE
        cxx_std_23
)
