#include <iostream>
#include <thread>
#include <chrono>

#include <Async/Async.hpp>
#include <Async/TaskLoop.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

// accept any Future or any
// create future
auto FetchData() -> Future {
  return std::async(std::launch::async, []() {
    auto threadId = std::this_thread::get_id();

    std::cout << "    [Async task] tick 1 " << threadId << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "    [Async task] tick 2 " << threadId << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "    [Async task] tick 3 " << threadId << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "    [Async task] tick 4 " << threadId << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return std::any{std::string(R"({ "name":"John", "age":30, "car":null })")};
  });
}

auto FetchDataUnsubscribe() -> std::any {
  auto threadId = std::this_thread::get_id();
  std::cout << "        [Async task] unsubscribe task " << threadId << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  return std::any{std::string(R"({ "task":"unsubscribe" })")};
}

void printResult(const std::string &result) {
  auto threadId = std::this_thread::get_id();
  std::cout << " _____________________" << std::endl;
  std::cout << "|       RESULT        |" << std::endl;
  std::cout << " ---------------------" << std::endl;
  std::cout << " got in thread << " << threadId << std::endl;
  std::cout << result << std::endl;
  std::cout << "" << std::endl;
}

void UsageExample() {
  TaskLoop::Instance().AddFuture(FetchData()).Then([](const std::any &result) {
    auto json = std::any_cast<std::string>(result);
    printResult(json);
  });
}

void UsageExampleWithUnsubscribe() {
  TaskLoop::Instance()
    .AddTask(FetchDataUnsubscribe)
    .Then([](const std::any &result) {
      auto json = std::any_cast<std::string>(result);
      printResult(json);
      printResult("Should not be in the console: " + json);
    })
    .UnsubscribeWhenThisCallbackProviderDestroyed();
}

auto UsageExampleWithUnsubscribe2() -> CallbackProviderBase {
  return TaskLoop::Instance()
    .AddTask(FetchDataUnsubscribe)
    .Then([](const std::any &result) {
      auto json = std::any_cast<std::string>(result);
      printResult("Should be in the console: " + json);
    })
    .UnsubscribeWhenThisCallbackProviderDestroyed();
}

auto fetchStuff() -> std::any {
  std::this_thread::sleep_for(std::chrono::seconds(4));
  return std::any{std::string(R"({body: "json from internet"})")};
}

auto fetchStuffFast() -> std::any {
  std::cout << "    [Async task] getting stuff fast " << std::this_thread::get_id() << std::endl;
  return std::any{std::string(R"({body: "fast json from internet"})")};
}


int main() {
  std::cout << "[Application] starting " << std::this_thread::get_id() << std::endl;
  UsageExample();
  UsageExample();
  UsageExampleWithUnsubscribe();
  auto asyncTaskId = UsageExampleWithUnsubscribe2();

  TaskLoop taskLoop;
  taskLoop
    .AddTask(fetchStuff)
    .Then([](const std::any &result) {
      auto json = std::any_cast<std::string>(result);
      printResult(json);
    });

  taskLoop
    .AddTask(fetchStuffFast)
    .Then([](const std::any &result) {
      auto json = std::any_cast<std::string>(result);
      printResult(json);
    });

  bool _runLoop = true;


  auto stopLoopLambda = [&](const std::any &result) {
    std::cout << "[Application] stopping the Main loop." << std::any_cast<int>(result)<< std::endl;
    if (std::any_cast<int>(result) == 99) {
      _runLoop = false;
    }
  };

  // test adding futures from a different thread.
  // futures are more flexible than task, because they can be parameterized
  std::thread thread([&]() {
    for (int i = 0; i < 100; ++i) {
      auto sleepLambda = std::async(std::launch::async, [](int duration) -> std::any {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return {duration};
      }, i);

      taskLoop
        .AddFuture(std::move(sleepLambda))
        .Then(stopLoopLambda);
    }
  });


  while (_runLoop) {
    std::cout << "[Main loop] tick  " << std::this_thread::get_id() << std::endl;
    taskLoop.Update();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  taskLoop.Update();

  thread.join();
  std::cout << "[Application] sopping " << std::this_thread::get_id() << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(4000));
  std::cout << "[Application] sopped " << std::this_thread::get_id() << std::endl;
  return 0;
}


#pragma clang diagnostic pop