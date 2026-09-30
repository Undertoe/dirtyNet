add_executable(sandbox_udp_many_posix_host
    main.cc
)

target_compile_features(sandbox_udp_many_posix_host
    PRIVATE
        cxx_std_23
)
