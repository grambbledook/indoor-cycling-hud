#pragma once

#include <functional>
#include <string>
#include "Events.h"

namespace Data {
    struct DataField {
        const std::string text;
        const std::function<std::string(MeasurementsUpdate)> value;

        DataField& operator=(const DataField &other) {
            if (this != &other) {
                const_cast<std::string&>(text) = other.text;
                const_cast<std::function<std::string(MeasurementsUpdate)>&>(value) = other.value;
            }
            return *this;
        }

        bool operator==(const DataField &other) const {
            return text == other.text;
        }
    };

    const auto HEART_RATE = DataField{
        "Heart rate",
        [](MeasurementsUpdate data) { return std::to_string(data.hrm.val); }
    };

    const auto CADENCE = DataField{
        "Cadence",
        [](MeasurementsUpdate data) { return std::to_string(data.cadence.val); }
    };

    const auto SPEED = DataField{
        "Speed",
        [](MeasurementsUpdate data) { return std::to_string(data.speed.val); }
    };

    const auto POWER = DataField{
        "Power",
        [](MeasurementsUpdate data) { return std::to_string(data.power.val); }
    };

    const std::vector DATA_FIELDS = {HEART_RATE, CADENCE, SPEED, POWER};

    inline int index(const DataField &field) {
        for (auto i = 0; i < DATA_FIELDS.size(); i++) {
            if (DATA_FIELDS[i].text == field.text) {
                return i;
            }
        }
        return -1;
    }
}
