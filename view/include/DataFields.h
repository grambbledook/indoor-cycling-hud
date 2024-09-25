#pragma once

#include <functional>
#include <string>

#include "ModelEvents.h"

namespace Data {
    inline std::string formatDuration(const long long durationMs) {
        const long long ms = durationMs % 1000;
        const long long total_seconds = durationMs / 1000;
        const long long seconds = total_seconds % 60;
        const long long total_minutes = total_seconds / 60;
        const long long minutes = total_minutes % 60;
        const long long hours = total_minutes / 60;

        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << hours << ":"
                << std::setfill('0') << std::setw(2) << minutes << ":"
                << std::setfill('0') << std::setw(2) << seconds << "."
                << std::setfill('0') << std::setw(3) << ms;

        return oss.str();
    }

    inline std::string formatDistance(const unsigned long distance, const DistanceUnit unit) {
        const double distanceKm = distance / 1000.0;

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << distanceKm << " " << distanceUnitToShortString(unit);
        return oss.str();
    }

    inline std::string toString(const std::optional<unsigned long> &value) {
        if (value.has_value()) {
            return std::to_string(value.value());
        }
        return "N/A";
    }

    inline std::string toString(const std::optional<double> &value) {
        if (value.has_value()) {
            return std::format("{:.1f}", value.value() * .01);
        }
        return "N/A";
    }

    struct DataField {
        const std::string text;
        const std::function<std::string(WorkoutEvent)> value;

        DataField &operator=(const DataField &other) {
            if (this != &other) {
                const_cast<std::string &>(text) = other.text;
                const_cast<std::function<std::string(WorkoutEvent)> &>(value) = other.value;
            }
            return *this;
        }

        bool operator==(const DataField &other) const {
            return text == other.text;
        }
    };

    const auto HEART_RATE = DataField{
        "Heart rate",
        [](const WorkoutEvent &event) { return toString(event.data.hrm); }
    };

    const auto AVG_HEART_RATE = DataField{
        "Avg Heart rate",
        [](const WorkoutEvent &event) { return toString(event.data.hrm); }
    };

    const auto CADENCE = DataField{
        "Cadence",
        [](const WorkoutEvent &event) { return toString(event.data.cadence); }
    };

    const auto AVG_CADENCE = DataField{
        "Avg Cadence",
        [](const WorkoutEvent &event) { return toString(event.data.cadence_avg); }
    };

    const auto SPEED = DataField{
        "Speed",
        [](const WorkoutEvent &event) { return toString(event.data.speed); }
    };

    const auto AVG_SPEED = DataField{
        "Avg Speed",
        [](const WorkoutEvent &event) { return toString(event.data.speed_avg); }
    };

    const auto POWER = DataField{
        "Power",
        [](const WorkoutEvent &event) { return std::to_string(event.data.power.value_or(0)); }
    };

    const auto AVG_POWER = DataField{
        "Avg Power",
        [](const WorkoutEvent &event) { return toString(event.data.power_avg); }
    };

    const auto AVG_3S_POWER = DataField{
        "3s Avg Power",
        [](const WorkoutEvent &event) { return toString(event.data.power_3s); }
    };

    const auto DURATION_WORKOUT = DataField{
        "Duration",
        [](const WorkoutEvent &event) { return formatDuration(event.durationMs); }
    };

    const auto DISTANCE_WORKOUT = DataField{
        "Distance",
        [](const WorkoutEvent &event) { return formatDistance(event.distance, event.distanceUnit); }
    };

    const std::vector DATA_FIELDS = {
        HEART_RATE, AVG_HEART_RATE, CADENCE, AVG_CADENCE, SPEED, AVG_SPEED, POWER, AVG_POWER, AVG_3S_POWER,
        DURATION_WORKOUT, DISTANCE_WORKOUT
    };

    inline int index(const DataField &field) {
        for (auto i = 0; i < DATA_FIELDS.size(); i++) {
            if (DATA_FIELDS[i].text == field.text) {
                return i;
            }
        }
        return -1;
    }

    struct SummaryData {
        const std::string text;
        const std::function<std::string(WorkoutEvent)> value;
    };

    const auto DURATION = SummaryData{
        "Duration",
        [](const WorkoutEvent &event) {
            return formatDuration(event.durationMs);
        }
    };

    const auto DISTANCE = SummaryData{
        "Distance",
        [](const WorkoutEvent &event) {
            return formatDistance(event.distance, event.distanceUnit);
        }
    };
}
