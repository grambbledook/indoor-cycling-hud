#include "EventBus.h"

#include <memory>
#include <spdlog/spdlog.h>
#include "ModelEvents.h"
//
auto EventBus::publish(const Event &event) -> void {
    const auto subscribers = this->subscribers[event.type];
    spdlog::trace("   Total event subscribers={}", subscribers.size());

    auto count = 1;
    for (const auto &subscriber: subscribers) {
        spdlog::trace("   Publishing event to subscriber [{}]", count);
        subscriber(event);
        spdlog::trace("   Done publishing event to subscriber [{}]", count);
        count++;
    }
}

auto EventBus::subscribe(const EventType type, const std::function<void(const Event &)> &receiver) -> void {
    std::lock_guard guard(mutex);

    subscribers[type].push_back(receiver);
}
