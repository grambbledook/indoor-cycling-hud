#pragma once

#include <QEvent>
#include "ModelEvents.h"

// this code poses danger as it is inlined and static variable might be creted in multiple translation units
inline QEvent::Type getDeviceDiscoveredType() {
    static const auto DeviceDiscoveredType = static_cast<QEvent::Type>(QEvent::registerEventType());
    return DeviceDiscoveredType;
}

inline QEvent::Type getDeviceSelectedType() {
    static const auto DeviceSelectedType = static_cast<QEvent::Type>(QEvent::registerEventType());
    return DeviceSelectedType;
}

inline QEvent::Type getSubscribedToServiceType() {
    static const auto SubscribedToServiceType = static_cast<QEvent::Type>(QEvent::registerEventType());
    return SubscribedToServiceType;
}

inline QEvent::Type getWorkoutEventType() {
    static const auto WorkoutEventType = static_cast<QEvent::Type>(QEvent::registerEventType());
    return WorkoutEventType;
}

inline QEvent::Type getWorkoutSummaryType() {
    static const auto WorkoutSummaryType = static_cast<QEvent::Type>(QEvent::registerEventType());
    return WorkoutSummaryType;
}

class DeviceDiscoveredEvent final : public QEvent {
public:
    explicit DeviceDiscoveredEvent(const DeviceDiscovered &event)
        : QEvent(getDeviceDiscoveredType()), event(event) {
    }

    [[nodiscard]] auto getEvent() const -> DeviceDiscovered { return event; }

private:
    DeviceDiscovered event;
};

class DeviceSelectedEvent final : public QEvent {
public:
    explicit DeviceSelectedEvent(const DeviceSelected &event)
        : QEvent(getDeviceSelectedType()), event(event) {
    }

    [[nodiscard]] auto getEvent() const -> DeviceSelected { return event; }

private:
    DeviceSelected event;
};

class SubscribedToServiceEvent final : public QEvent {
public:
    explicit SubscribedToServiceEvent(const SubscribedToService &event)
        : QEvent(getSubscribedToServiceType()), event(event) {
    }

    [[nodiscard]] auto getEvent() const -> SubscribedToService { return event; }

private:
    SubscribedToService event;
};

class WorkoutEventEvent final : public QEvent {
public:
    explicit WorkoutEventEvent(const WorkoutEvent &data)
        : QEvent(getWorkoutEventType()), data(data) {
    }

    [[nodiscard]] auto getData() const -> WorkoutEvent { return data; }

private:
    WorkoutEvent data;
};

class WorkoutSummaryEvent final : public QEvent {
public:
    explicit WorkoutSummaryEvent(const WorkoutEvent &data)
        : QEvent(getWorkoutSummaryType()), data(data) {
    }

    [[nodiscard]] auto getData() const -> WorkoutEvent { return data; }

private:
    WorkoutEvent data;
};
