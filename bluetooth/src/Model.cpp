#include <algorithm>
#include <iostream>

#include "Events.h"
#include "Model.h"

constexpr auto MM_TO_KM = 1.0 / 1000000;
constexpr auto MS_TO_MIN = 1.0 / (60 * 1024);
constexpr auto MS_TO_HOUR = 1.0 / (60 * 60 * 1024);
constexpr auto DEFAULT_TIRE_CIRCUMFERENCE_MM = 2168;

template<DerivedFromMeasurement T>
void Model::recordData(MeasurementEvent<T> event) {
    if (auto *data = dynamic_cast<HrmMeasurement *>(&event.measurement)) {
        hrmState.recordMetric(data->hrm);
        hrmState.aggregateMetric(data->hrm);

        std::cout << "HRM: " << hrmState.stats.latest << " AVG: " << hrmState.stats.average << std::endl;
    } else if (auto *data = dynamic_cast<CadenceMeasurement *>(&event.measurement)) {
        cadenceState.recordMetric(std::pair{data->ccr, data->lcet});

        auto events = cadenceState.getLastN(2);
        auto [prevCcr, prevLcet, ccr, lcet] = std::tuple_cat(events[0], events[1]);

        if (lcet == prevLcet) {
            cadenceState.unrecordMetric();
            return;
        }

        const auto lcetResetCorrection = lcet > prevLcet ? 0 : 0x10000;

        const auto totalRevolutions = ccr - prevCcr;
        const auto timeDelta = lcet - prevLcet + lcetResetCorrection;

        const auto cadence = totalRevolutions / (timeDelta * MS_TO_MIN);

        cadenceState.aggregateMetric(std::round(cadence));

        std::cout << "CADENCE: " << cadenceState.stats.latest << " AVG: " << cadenceState.stats.average << std::endl;
    } else if (auto *data = dynamic_cast<SpeedMeasurement *>(&event.measurement)) {
        speedState.recordMetric(std::pair{data->cwr, data->lwet});

        auto events = speedState.getLastN(2);
        auto [prevCwr, prevLwet, cwr, lwet] = std::tuple_cat(events[0], events[1]);

        if (lwet == prevLwet) {
            speedState.unrecordMetric();
            return;
        }

        const auto lwetResetCorrection = lwet > prevLwet ? 0 : 0x10000;

        const auto totalRevolutions = cwr - prevCwr;
        const auto timeDelta = lwet - prevLwet + lwetResetCorrection;

        const auto totalKmh = totalRevolutions * DEFAULT_TIRE_CIRCUMFERENCE_MM * MM_TO_KM;
        const auto speed = totalKmh / timeDelta * MS_TO_HOUR;

        speedState.aggregateMetric(std::round(speed));

        std::cout << "SPEED: " << speedState.stats.latest << " AVG: " << speedState.stats.average << std::endl;
    } else if (const auto *data = dynamic_cast<PowerMeasurement *>(&event.measurement)) {
        powerState.recordMetric(data->power);
        powerState.aggregateMetric(data->power);

        std::cout << "POWER: " << powerState.stats.latest << " AVG: " << powerState.stats.average << std::endl;
    } else {
        std::cout << "Unknow event received: " << typeid(event.measurement).name() << std::endl;
    }
}


template void Model::recordData<HrmMeasurement>(MeasurementEvent<HrmMeasurement> event);

template void Model::recordData<SpeedMeasurement>(MeasurementEvent<SpeedMeasurement> event);

template void Model::recordData<CadenceMeasurement>(MeasurementEvent<CadenceMeasurement> event);

template void Model::recordData<PowerMeasurement>(MeasurementEvent<PowerMeasurement> event);
