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
ReAction/Menubar.cpp
)

set(SIMPLEDEBUG_SOURCES
SimpleDebug/Binary.cpp
SimpleDebug/Breaks.cpp
SimpleDebug/Handle.cpp
SimpleDebug/LowLevel.cpp
SimpleDebug/Pipe.cpp
SimpleDebug/Process.cpp
SimpleDebug/Stacktracer.cpp
SimpleDebug/Strings.cpp
SimpleDebug/Symbols.cpp
SimpleDebug/TextFile.cpp
SimpleDebug/Tracer.cpp
)

set(SPOTLESS_SOURCES main.cpp Spotless/Spotless.cpp)
set(SOURCES ${SPOTLESS_SOURCES} ${REACTION_SOURCES} ${SIMPLEDEBUG_SOURCES})

add_executable(Spotless ${SOURCES})
target_link_libraries(Spotless -lauto)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -athread=native -gstabs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gstabs")