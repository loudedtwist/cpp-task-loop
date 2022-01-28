#pragma once

#include <any>
#include <functional>
#include <future>
#include <map>
#include <vector>
#include <utility>
#include <iostream>

#include <Async/Async.hpp>
#include <Async/CallbackProvider.hpp>
#include <Async/Utils.hpp>
#include <IObservable.hpp>


/// Use message loop only from the main thread.
/// Add tasks or _futures from main thread.
/// They will be executed in a different thread,
/// but the callback will called from the main thread again.
/// Task / Future that has no callback registered, will be also deleted.
/// Also it could be change to wait, until there is a recipient.
class TaskLoop : public IObservable {
public:
  virtual ~TaskLoop();

  static auto Instance() -> TaskLoop&;

  auto AddFuture(Future&& future) -> CallbackProvider;
  auto AddTask(const Task &task) -> CallbackProvider;
  void AddCallback(utils::unique_id id, const Callback &callback) override;
  void RemoveCallback(utils::unique_id id) override;

  void Update();

private:
  std::map<utils::unique_id, Future> _futures;
  std::map<utils::unique_id, Callback> _callbacks;
  std::mutex _mtx;
};
