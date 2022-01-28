#include <Async/TaskLoop.hpp>

auto TaskLoop::AddFuture(Future&& future) -> CallbackProvider {
  std::unique_lock<std::mutex> lk(_mtx);
  auto id = utils::GenerateTimestampID();
  _futures.emplace(id, std::move(future));
  return CallbackProvider{id, *this};
}

auto TaskLoop::AddTask(const Task& task) -> CallbackProvider {
  std::unique_lock<std::mutex> lk(_mtx);
  auto id = utils::GenerateTimestampID();
  _futures.emplace(id, std::async(std::launch::async, task));
  return CallbackProvider{id, *this};
}

void TaskLoop::AddCallback(utils::unique_id id, const Callback& callback) {
  std::unique_lock<std::mutex> lk(_mtx);
  _callbacks.emplace(id, callback);
}

void TaskLoop::RemoveCallback(utils::unique_id id) {
  std::unique_lock<std::mutex> lk(_mtx);
  _callbacks.erase(id);
}

void TaskLoop::Update() {
  std::unique_lock<std::mutex> lk(_mtx);
  std::cout << "[Message loop] tick " << std::this_thread::get_id() << " " << _futures.size() << std::endl;
  std::vector<utils::unique_id> finishedFutures;

  for (auto&[id, future]: _futures) {
    // invalid future
    if (!future.valid()) {
      finishedFutures.push_back(id);
      std::cerr << "[Message loop] future with id " << id << " is not valid. Its shared state was already consumed."
                << std::endl;
      continue;
    }

    // valid future and its value is ready
    if (async::is_ready(future)) {
      if (_callbacks.find(id) != _callbacks.end()) {
        auto value = future.get();
        _callbacks[id](value);
      }
      finishedFutures.push_back(id);
    }
  }

  for (const auto &finishedId: finishedFutures) {
    _futures.erase(finishedId);
    _callbacks.erase(finishedId);
  }
}

auto TaskLoop::Instance() -> TaskLoop& {
  static TaskLoop messageLoopGlobalStaticVariable;
  return messageLoopGlobalStaticVariable;
}

TaskLoop::~TaskLoop() {
  std::cout << "Destroyng TaskLoop. [size] "<< _futures.size() << std::endl;
}
