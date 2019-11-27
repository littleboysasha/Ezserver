cmake_minimum_required(VERSION 3.15)
project(ezserver)

set(CMAKE_CXX_STANDARD 11)

link_directories(C:\\Program Files\\Environments\\MinGw\\mingw64\\x86_64-w64-mingw32\\lib)
link_libraries(libws2_32.a)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_executable(ezserver main.cpp Listener.cpp Listener.h)


