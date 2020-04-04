set(REMOTE_HOST 192.168.1.135)
set(REMOTE_PORT 10023)
set(REMOTE_USER amigaX1000)
set(REMOTE_FTPPASSWORD amigaftp)
set(REMOTE_TELNETPASSWORD amigatelnet)

set(TEST_SCRIPT "testonremote.sh")
set(EXPECT_SCRIPT "runonremote.expect")
configure_file("Modules/${TEST_SCRIPT}" ${TEST_SCRIPT} COPYONLY)
configure_file("Modules/${EXPECT_SCRIPT}" ${EXPECT_SCRIPT} COPYONLY)

add_custom_target(remote_test_target ALL)

function(configure_remote)
    set(oneValueArgs HOST_ADDRESS PORT_NUMBER USER_LOGIN FTP_PASSWORD TELNET_PASSWORD)
    cmake_parse_arguments(configure_remote "" "${oneValueArgs}" "" ${ARGN})

    set(REMOTE_HOST ${configure_remote_HOST_ADDRESS})
    set(REMOTE_PORT ${configure_remote_PORT_NUMBER})
    set(REMOTE_USER ${configure_remote_USER_LOGIN})
    set(REMOTE_FTPPASSWORD ${configure_remote_FTP_PASSWORD})
    set(REMOTE_TELNETPASSWORD ${configure_remote_TELNET_PASSWORD})
endfunction()

function(add_remote_test)
    set(options "")
    set(oneValueArgs TEST ARGUMENTS COMPARE)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(add_remote_test "${options}" "${oneValueArgs}"
                          "${multiValueArgs}" ${ARGN} )

    set(TESTNAME REMOTE_TEST_${add_remote_test_TEST})
    add_executable(${TESTNAME} ${add_remote_test_SOURCES})
    if(AMIGA)
        target_link_libraries(${TESTNAME} -lauto)
    endif()
    add_dependencies(remote_test_target ${TESTNAME})

    add_custom_command(TARGET remote_test_target
        POST_BUILD
        COMMAND bash ${TEST_SCRIPT} 
        ${add_remote_test_TEST}
        "${TESTNAME} ${add_remote_test_ARGUMENTS}" #${add_remote_test_COMMAND}
        ${REMOTE_HOST}
        ${REMOTE_PORT}
        ${REMOTE_USER}
        ${REMOTE_FTPPASSWORD}
        ${REMOTE_TELNETPASSWORD}
        ${add_remote_test_COMPARE}
    )

endfunction()
