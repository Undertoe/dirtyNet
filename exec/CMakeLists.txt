add_executable(dirtynet_exec
    main.cc
)

target_link_libraries(dirtynet_exec
    PRIVATE
        dirtyNet::dirtynet
)
