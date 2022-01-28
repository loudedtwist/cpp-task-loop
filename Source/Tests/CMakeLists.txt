#find_package(Catch2 REQUIRED)
Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.0-preview3
)

FetchContent_MakeAvailable(Catch2)

# These tests need their own main
add_executable(tests AsyncTest.cpp)
target_link_libraries(tests Catch2::Catch2WithMain)
target_link_libraries(tests Async)