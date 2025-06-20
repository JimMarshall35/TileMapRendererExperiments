include(CMakePrintHelpers)
cmake_minimum_required(VERSION 3.25)
project(StardewEngine)
find_package(glfw3 REQUIRED)
find_package(box2d)
find_package(Lua51 REQUIRED)
find_package(Freetype REQUIRED)
find_package(LibXml2 REQUIRED)

add_subdirectory(src)


target_link_libraries(StardewEngine PRIVATE
  cglm_headers)


target_link_libraries(StardewEngine PRIVATE glfw)

add_subdirectory(lib/cglm-0.9.6 EXCLUDE_FROM_ALL)


target_include_directories(StardewEngine  PRIVATE include)
target_include_directories(StardewEngine  PRIVATE lib/glad/include)

target_include_directories(StardewEngine PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_include_directories(StardewEngine PRIVATE ${LUA_INCLUDE_DIR})
target_include_directories(StardewEngine PRIVATE ${LIBXML2_INCLUDE_DIR})

target_link_libraries(StardewEngine PRIVATE ${FREETYPE_LIBRARIES})
target_link_libraries(StardewEngine PRIVATE ${LUA_LIBRARIES})
target_link_libraries(StardewEngine PRIVATE ${LIBXML2_LIBRARIES})
target_link_libraries(StardewEngine PRIVATE m) 