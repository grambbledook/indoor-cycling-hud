//
// Created by denis on 10/22/2024.
//

#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H
#include <mutex>

#include "ModelEvents.h"

class EventBus {
public:
    EventBus() = default;

    ~EventBus() = default;

    auto publish(const Event &event) -> void;

    auto subscribe(const EventType type, const std::function<void(const Event &)> &receiver) -> void;

private:
    std::mutex mutex;
    std::unordered_map<EventType, std::vector<std::function<void(const Event &)>> > subscribers;
};


#endif //MESSAGEBUS_H
