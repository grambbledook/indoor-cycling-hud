//
// Created by denis on 10/22/2024.
//

#ifndef MESSAGEBUS_H
#define MESSAGEBUS_H
#include "ModelEvents.h"


class EventBus {
public:
    EventBus() = default;

    ~EventBus() = default;

    auto publish(const Event &event) -> void;
};


#endif //MESSAGEBUS_H
