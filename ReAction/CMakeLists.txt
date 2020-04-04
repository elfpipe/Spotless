include(Modules/CMakeRemoteTest.cmake)

set(REACTION_SOURCES
    listbrowser.cpp
    mainwindow.cpp
    requester.cpp
    speedbar.cpp
    progress.cpp
    layout.cpp
    widget.cpp
    screen.cpp
    button.cpp
    panel.cpp
    menu.cpp
    ../Service/stringtools.cpp
)

if(CMAKE_CROSSCOMPILING)

    add_remote_test(TEST ReactionClassesTest
        ARGUMENTS ""
        COMPARE "All OK"
        SOURCES
        Tests/test.cpp
        ${REACTION_SOURCES}
    )

endif()
