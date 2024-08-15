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

inline QEvent::Type getWorkoutDataType() {
    static const auto WorkoutDataType = static_cast<QEvent::Type>(QEvent::registerEventType());
    return WorkoutDataType;
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

    [[nodiscard]] DeviceDiscovered getEvent() const { return event; }

private:
    DeviceDiscovered event;
};

class DeviceSelectedEvent final : public QEvent {
public:
    explicit DeviceSelectedEvent(const DeviceSelected &event)
        : QEvent(getDeviceSelectedType()), event(event) {
    }

    [[nodiscard]] DeviceSelected getEvent() const { return event; }

private:
     DeviceSelected event;
};

class WorkoutDataEvent final : public QEvent {
public:
    explicit WorkoutDataEvent(WorkoutData data)
        : QEvent(getWorkoutDataType()), data(data) {
    }

    [[nodiscard]] WorkoutData getData() const { return data; }

private:
    WorkoutData data;
};

class WorkoutSummaryEvent final : public QEvent {
public:
    explicit WorkoutSummaryEvent(WorkoutSummary data)
        : QEvent(getWorkoutSummaryType()), data(data) {
    }

    [[nodiscard]] WorkoutSummary getData() const { return data; }

private:
    WorkoutSummary data;
};
