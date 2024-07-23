#pragma once

#include <functional>
#include "Data.h"
#include "Model.h"

template<typename T>
class Channel {
public:
    void subscribe(std::function<void(T)> receiver) {
        receivers.push_back(receiver);
    }

    void publish(T data) {
        for (const auto &receiver: receivers) {
            receiver(data);
        }
    }

private:
    std::vector<std::function<void(T)> > receivers;
};

template<typename T>
class Notifications {
public:
    Channel<T> data;
    Channel<std::shared_ptr<Device> > deviceDiscovered;
    Channel<std::shared_ptr<Device> > deviceSelected;
};
