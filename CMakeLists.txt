cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ImageApp)

add_subdirectory(CoreGraphics CoreGraphics)

file(GLOB_RECURSE APP_SOURCE src/*.cpp src/*.hpp src/*.h)
 
add_executable(${PROJECT_NAME} ${APP_SOURCE})

target_link_libraries(${PROJECT_NAME}
    PRIVATE CoreGraphics
)


