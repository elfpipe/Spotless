cmake_minimum_required(VERSION 3.0)

project(Spotless)

set(REACTION_SOURCES
ReAction/Listbrowser.cpp
ReAction/MainWindow.cpp
ReAction/Requester.cpp
ReAction/Speedbar.cpp
ReAction/Progress.cpp
ReAction/Layout.cpp
ReAction/Widget.cpp
ReAction/Screen.cpp
ReAction/GoButton.cpp
ReAction/Panel.cpp
ReAction/Menubar.cpp)

set(SPOTLESS_SOURCES main.cpp Spotless/Spotless.cpp)
set(SOURCES ${SPOTLESS_SOURCES} ${REACTION_SOURCES} SimpleDebug/Strings.cpp)

add_executable(Spotless ${SOURCES})
target_link_libraries(Spotless -lauto)