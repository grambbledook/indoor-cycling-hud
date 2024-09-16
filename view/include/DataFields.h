#pragma once

#include <functional>
#include <string>

#include "ModelEvents.h"

namespace Data {

    inline std::string formatDuration(long long durationMs) {
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

    inline std::string formatDistance(const long distance, const DistanceUnit unit) {
        const long long kms = distance / 1000;
        const long long meters = distance % 1000;

        std::ostringstream oss;
        oss << kms << "." << meters << " " << distanceUnitToShortString(unit);
        return oss.str();
    }

    struct DataField {
        const std::string text;
        const std::function<std::string(WorkoutData)> value;

        DataField &operator=(const DataField &other) {
            if (this != &other) {
                const_cast<std::string &>(text) = other.text;
                const_cast<std::function<std::string(WorkoutData)> &>(value) = other.value;
            }
            return *this;
        }

        bool operator==(const DataField &other) const {
            return text == other.text;
        }
    };

    const auto HEART_RATE = DataField{
        "Heart rate",
        [](const WorkoutData &data) { return std::to_string(data.hrm.val); }
    };

    const auto AVG_HEART_RATE = DataField{
        "Avg Heart rate",
        [](const WorkoutData &data) { return std::to_string(data.hrm.avg); }
    };

    const auto CADENCE = DataField{
        "Cadence",
        [](const WorkoutData &data) { return std::to_string(data.cadence.val); }
    };

    const auto AVG_CADENCE = DataField{
        "Avg Cadence",
        [](const WorkoutData &data) { return std::to_string(data.cadence.avg); }
    };

    const auto SPEED = DataField{
        "Speed",
        [](const WorkoutData &data) { return std::format("{:.1f}", data.speed.val * .01); }
    };

    const auto AVG_SPEED = DataField{
        "Avg Speed",
        [](const WorkoutData &data) { return std::format("{:.1f}", data.speed.avg * .01); }
    };

    const auto POWER = DataField{
        "Power",
        [](const WorkoutData &data) { return std::to_string(data.power.val); }
    };

    const auto AVG_POWER = DataField{
        "Avg Power",
        [](const WorkoutData &data) { return std::to_string(data.power.avg); }
    };

    const auto AVG_3S_POWER = DataField{
        "3s Avg Power",
        [](const WorkoutData &data) { return std::to_string(data.power.windowedAvg); }
    };

    const auto DURATION_WORKOUT = DataField{
        "Duration",
        [](const WorkoutData &data) { return formatDuration(data.duration); }
    };

    const auto DISTANCE_WORKOUT = DataField{
        "Distance",
        [](const WorkoutData &data) { return formatDistance(data.distance, data.distanceUnit); }
    };

    const std::vector DATA_FIELDS = {
        HEART_RATE, AVG_HEART_RATE, CADENCE, AVG_CADENCE, SPEED, AVG_SPEED, POWER, AVG_POWER, AVG_3S_POWER, DURATION_WORKOUT, DISTANCE_WORKOUT
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
        [](const WorkoutEvent &data) {
            return formatDuration(data.durationMs);
        }
    };

    const auto DISTANCE = SummaryData{
        "Distance",
        [](const WorkoutEvent &data) {
            return formatDistance(data.distance, data.distanceUnit);
        }
    };

}
