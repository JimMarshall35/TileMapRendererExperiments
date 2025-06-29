include(CMakePrintHelpers)
cmake_minimum_required(VERSION 3.25)
project(StardewEngine)
find_package(glfw3 REQUIRED)
find_package(box2d)
find_package(Lua REQUIRED)
find_package(Freetype REQUIRED)
find_package(LibXml2 REQUIRED)

if(Lua_FOUND AND NOT TARGET Lua::Lua)
  add_library(Lua::Lua INTERFACE IMPORTED)
  set_target_properties(
    Lua::Lua
    PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}"
      INTERFACE_LINK_LIBRARIES "${LUA_LIBRARIES}"
  )
endif()

add_subdirectory(src)


target_link_libraries(StardewEngine PUBLIC
  cglm_headers)


target_link_libraries(StardewEngine PUBLIC glfw)

add_subdirectory(lib/cglm-0.9.6 EXCLUDE_FROM_ALL)


target_include_directories(StardewEngine  PUBLIC include)
target_include_directories(StardewEngine  PRIVATE lib/glad/include)
cmake_print_variables(FREETYPE_INCLUDE_DIRS)
target_include_directories(StardewEngine PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_include_directories(StardewEngine PUBLIC ${LUA_INCLUDE_DIR})
target_include_directories(StardewEngine PUBLIC ${LIBXML2_INCLUDE_DIR})
target_include_directories(StardewEngine PUBLIC ${BOX2D_INCLUDE_DIR})

target_link_libraries(StardewEngine PUBLIC ${FREETYPE_LIBRARIES})
target_link_libraries(StardewEngine PUBLIC ${LUA_LIBRARIES})
target_link_libraries(StardewEngine PUBLIC ${LIBXML2_LIBRARIES})
target_link_libraries(StardewEngine PUBLIC ${BOX2D_LIBRARIES})

target_link_libraries(StardewEngine PUBLIC Lua::Lua) 