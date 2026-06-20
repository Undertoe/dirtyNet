add_executable(dirtynet_exec_basic
    main.cc
)

target_link_libraries(dirtynet_exec_basic
    PRIVATE
        dirtyNet::dirtynet
)
