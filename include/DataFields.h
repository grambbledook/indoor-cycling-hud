#pragma once

#include <functional>
#include <string>

#include "ModelEvents.h"

namespace Data {
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
        "avg. Heart rate",
        [](const WorkoutData &data) { return std::to_string(data.hrm.avg); }
    };

    const auto CADENCE = DataField{
        "Cadence",
        [](const WorkoutData &data) { return std::to_string(data.cadence.val); }
    };

    const auto AVG_CADENCE = DataField{
        "avg. Cadence",
        [](const WorkoutData &data) { return std::to_string(data.cadence.avg); }
    };

    const auto SPEED = DataField{
        "Speed",
        [](const WorkoutData &data) { return std::to_string(data.speed.val); }
    };

    const auto AVG_SPEED = DataField{
        "avg. Speed",
        [](const WorkoutData &data) { return std::to_string(data.speed.avg); }
    };

    const auto POWER = DataField{
        "Power",
        [](const WorkoutData &data) { return std::to_string(data.power.val); }
    };

    const std::vector DATA_FIELDS = {HEART_RATE, AVG_HEART_RATE, CADENCE, AVG_CADENCE, SPEED, AVG_SPEED, POWER};

    inline int index(const DataField &field) {
        for (auto i = 0; i < DATA_FIELDS.size(); i++) {
            if (DATA_FIELDS[i].text == field.text) {
                return i;
            }
        }
        return -1;
    }
}
