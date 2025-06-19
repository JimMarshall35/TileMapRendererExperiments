cmake_minimum_required(VERSION 3.25)
project(StardewEngine)
find_package(glfw3)
find_package(box2d)
add_subdirectory(lib/freetype-2.13.3)

add_subdirectory(src)

target_link_libraries(StardewEngine PRIVATE freetype)

target_link_libraries(StardewEngine PRIVATE
  cglm_headers)


target_link_libraries(StardewEngine PRIVATE glfw)

add_subdirectory(lib/cglm-0.9.6 EXCLUDE_FROM_ALL)

add_subdirectory(lib/lua-cmake)

target_link_libraries(StardewEngine PRIVATE lua::lib)
target_link_libraries(StardewEngine PRIVATE m) 