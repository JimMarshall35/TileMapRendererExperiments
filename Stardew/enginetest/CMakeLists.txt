cmake_minimum_required(VERSION 3.25)

find_package(GTest REQUIRED)

enable_testing()

add_executable(
  StardewEngineTest
  DynArrayTests.cpp
  ObjectPoolTests.cpp
  GameFrameworkTests.cpp
  SharedPtrTests.cpp
)
if(WIN32)
target_link_libraries(
  StardewEngineTest
  PRIVATE
  gtest::gtest
)
else()
target_link_libraries(
  StardewEngineTest
  PRIVATE
  GTest::gtest_main
)
endif()


if(WIN32)

else()
include(GoogleTest)

gtest_discover_tests(StardewEngineTest)

endif()
