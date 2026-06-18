add_executable(dirtynet_exec_basic
    main.cpp
)

target_link_libraries(dirtynet_exec_basic
    PRIVATE
        dirtyNet::dirtynet
)
