cmake_minimum_required(VERSION 3.25)
project(StardewEngineTest)

find_package(GTest REQUIRED)

enable_testing()

add_executable(
  StardewEngineTest
  main.cpp
)
target_link_libraries(
  StardewEngineTest
  PRIVATE
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(StardewEngineTest)
