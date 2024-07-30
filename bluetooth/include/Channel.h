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
