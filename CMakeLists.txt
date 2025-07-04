cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(DXProj VERSION 0.1.0 LANGUAGES C CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

file(GLOB SRCS src/*.cpp)

qt_add_executable(DXProj ${SRCS})

target_link_libraries(DXProj PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)

