include(CMakePrintHelpers)
cmake_minimum_required(VERSION 3.25)
project(WarFarmer)
add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(enginetest)

target_link_libraries(WarFarmer PUBLIC StardewEngine)
target_link_libraries(StardewEngineTest PUBLIC StardewEngine)
