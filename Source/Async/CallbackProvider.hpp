#pragma once

#include <iostream>

#include <IObservable.hpp>
#include <Async/Utils.hpp>

struct CallbackProviderBase {
public:
    CallbackProviderBase(const CallbackProviderBase &) = delete;
    CallbackProviderBase &operator=(const CallbackProviderBase &) = delete;

    CallbackProviderBase(utils::unique_id id, IObservable &hasCallback)
            : id(id), _messageLoop(hasCallback) {}

    CallbackProviderBase(utils::unique_id id, IObservable &hasCallback, bool unsubscribeAfterDestroyed)
            : id(id), _messageLoop(hasCallback), _unsubscribeAfterDestroyed(unsubscribeAfterDestroyed) {}

    virtual ~CallbackProviderBase() {
        if (_unsubscribeAfterDestroyed) {
            _messageLoop.RemoveCallback(id);
        }
    }

    /// If called after Then, this object will automatically unsubscribe
    /// from message loop callback after it's (CallbackProvider) destroyed.
    /// @returns self
    auto UnsubscribeWhenThisCallbackProviderDestroyed() -> CallbackProviderBase {
        return CallbackProviderBase{id, _messageLoop, true};
    }

    utils::unique_id id;

protected:
    IObservable &_messageLoop;
    bool _unsubscribeAfterDestroyed{false};
};


struct CallbackProvider : public CallbackProviderBase {
public:

    CallbackProvider(utils::unique_id id, IObservable &hasCallback)
            : CallbackProviderBase(id, hasCallback) {}

    /// Register callback for a task or future
    /// It will be called on the main thread
    /// @param callback
    /// @returns self
    auto Then(const Callback &callback) -> CallbackProviderBase {
        _messageLoop.AddCallback(id, callback);
        return CallbackProviderBase{id, _messageLoop};
    }
};