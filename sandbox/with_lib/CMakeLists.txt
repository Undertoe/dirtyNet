add_executable(sandbox_with_lib
    main.cc
)

target_link_libraries(sandbox_with_lib
    PRIVATE
        dirtyNet::dirtynet
)
