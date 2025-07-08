cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(DXProj VERSION 0.1.0 LANGUAGES C CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

file(GLOB SRCS src/*.cpp)

qt_add_executable(DXProj ${SRCS})

target_include_directories(DXProj SYSTEM BEFORE PRIVATE "C:/Program Files (x86)/Windows Kits/10/Include")

target_link_libraries(DXProj PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui d3dcompiler)

# {
#     "version": 8,
#     "configurePresets": [
#         {
#             "name": "Preset",
#             "displayName": "GCC 15.1.0 x86_64-w64-mingw32",
#             "description": "Using compilers: C = C:\\MinGW\\bin\\gcc.exe, CXX = C:\\MinGW\\bin\\g++.exe",
#             "generator": "MinGW Makefiles",
#             "binaryDir": "${sourceDir}/out/build/${presetName}",
#             "cacheVariables": {
#                 "CMAKE_INSTALL_PREFIX": "${sourceDir}/out/install/${presetName}",
#                 "CMAKE_C_COMPILER": "C:/MinGW/bin/gcc.exe",
#                 "CMAKE_CXX_COMPILER": "C:/MinGW/bin/g++.exe",
#                 "CMAKE_BUILD_TYPE": "Debug",
#                 "CMAKE_EXPORT_COMPILE_COMMANDS": "1"
#             }
#         }
#     ]
# }