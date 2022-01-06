# Chapter 1 CmakeLists

# TODO figure out how to get list of files and just compile them individually as 
# binaries called file name

function (add_exec name file)
    add_executable(${name} ${file})
    set_target_properties(${name}
                      PROPERTIES 
                      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/ch1)
    target_link_libraries(${name}
                          PRIVATE zmq)
    target_include_directories(${name}
                               PRIVATE ${PROJECT_SOURCE_DIR}/include)
endfunction()

add_exec(hwserver hwserver.c)
add_exec(hwclient hwclient.c)
add_exec(version version.c)
add_exec(wuserver wuserver.c)
add_exec(wuclient wuclient.c)
