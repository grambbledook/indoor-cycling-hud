#pragma once

#include <functional>
#include "Data.h"

template<typename T>
class Channel {
public:
    auto subscribe(const std::function<void(T)> &receiver) const -> void {
        receivers.push_back(receiver);
    }

    auto publish(T data) -> void {
        for (const auto &receiver: receivers) {
            receiver(data);
        }
    }

private:
    mutable std::vector<std::function<void(T)> > receivers;
};
