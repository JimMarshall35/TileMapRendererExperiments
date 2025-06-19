cmake_minimum_required(VERSION 3.17)
project(MyProject C)

# Setup libraries
include(FetchContent)
FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG master)
FetchContent_Declare(glad GIT_REPOSITORY https://github.com/SasLuca/MyGlad.git GIT_TAG master)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_MakeAvailable(glad)

# Setup executable
add_executable(${PROJECT_NAME} "sources/main.c")
target_include_directories(${PROJECT_NAME} PUBLIC "source")
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)