#pragma once

#include <any>
#include <future>
#include <functional>
#include <Async/Utils.hpp>


using MessageType = std::any;
using Future = std::future<MessageType>;
using Callback = std::function<void(MessageType)>;
using Task = std::function<MessageType(void)>;

struct IObservable {
    virtual void AddCallback(utils::unique_id id, const Callback &callback) = 0;
    virtual void RemoveCallback(utils::unique_id id) = 0;
};