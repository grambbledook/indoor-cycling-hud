#pragma once

#include <functional>
#include "Data.h"

template<typename T>
class Channel {
public:
    void subscribe(const std::function<void(T)> &receiver) const{
        receivers.push_back(receiver);
    }

    void publish(T data) {
        for (const auto &receiver: receivers) {
            receiver(data);
        }
    }

private:
    mutable std::vector<std::function<void(T)> > receivers;
};

template<typename T>
class Notifications {
public:
    Channel<T> data;
    Channel<std::shared_ptr<Device> > deviceDiscovered;
    Channel<std::shared_ptr<Device> > deviceSelected;
};
