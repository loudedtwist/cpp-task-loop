#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <thread>
#include <chrono>

#include <Async/Async.hpp>
#include <Async/TaskLoop.hpp>

unsigned int Factorial( unsigned int number ) {
  return number <= 1 ? number : Factorial(number-1)*number;
}
void printResult(const std::string& result) {
  std::cout << "Result: " << result << std::endl;
  std::cout << "  - thread: " << std::this_thread::get_id() << std::endl;
}

TEST_CASE( "Do not call callbacks when message loop is destroyed.", "[Async]" ) {
  auto isThenCalled = false;
  {
    MessageLoop messageLoop;
    messageLoop
      .AddTask([]() -> std::any {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Future almost finished" << std::endl;
        return {std::string("success")};
      })
      ;
      // .Then([&](const std::any& result) {
      //   auto json = std::any_cast<std::string>(result);
      //   printResult(json);
      //   isThenCalled = true;
      // });
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    messageLoop.Update();
  }

  REQUIRE(!isThenCalled);
  REQUIRE( Factorial(1) == 1 );
}