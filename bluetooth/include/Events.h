#pragma once

#include <memory> // For std::shared_ptr and std::move
#include <string> // For std::string
#include <type_traits> // For std::is_base_of_v

#include "Data.h"

class Measurement {
public:
    virtual ~Measurement() = default;
};

template<typename T>
concept DerivedFromMeasurement = std::is_base_of_v<Measurement, T>;

template<typename T>
    requires DerivedFromMeasurement<T>
class MeasurementEvent {
public:
    explicit MeasurementEvent(const std::shared_ptr<Device> &device, T measurement)
        : device(device), measurement(measurement) {
    }

    std::shared_ptr<Device> device;
    T measurement;
};

struct HrmMeasurement final : Measurement {
    explicit HrmMeasurement(const int hrm): Measurement(), hrm(hrm) {
    }

    unsigned int hrm;
};

struct SpeedMeasurement final : Measurement {
    explicit SpeedMeasurement(const int cwr, const int lwet): Measurement(), cwr(cwr), lwet(lwet) {
    }

    unsigned int cwr;
    unsigned int lwet;
};

struct CadenceMeasurement final : Measurement {
    CadenceMeasurement(const int ccr, const int lcet): Measurement(), ccr(ccr), lcet(lcet) {
    }

    unsigned int ccr;
    unsigned int lcet;
};

struct PowerMeasurement final : Measurement {
    explicit PowerMeasurement(const int power): Measurement(), power(power) {
    }

    unsigned int power;
};

class FecMeasurement : public Measurement {
};

struct FeState final : FecMeasurement {
    explicit FeState(std::string feState, const bool lapToggle)
        : feState(std::move(feState)), lapToggle(lapToggle) {
    }

    std::string feState;
    bool lapToggle;
};

struct GeneralData final : FecMeasurement {
    explicit GeneralData(const int elapsedTime, const int distanceTraveled, const int speed, const int heartRate,
                         FeState feState)
        : elapsed_time(elapsedTime), distance_traveled(distanceTraveled), speed(speed), heart_rate(heartRate),
          feState(std::move(feState)) {
    }

    int elapsed_time;
    int distance_traveled;
    int speed;
    int heart_rate;
    FeState feState;
};

struct GeneralSettings final : FecMeasurement {
    explicit GeneralSettings(const int cycleLength, const int incline, const int resistance, FeState feState)
        : cycle_length(cycleLength), incline(incline), resistance(resistance),
          feState(std::move(feState)) {
    }

    int cycle_length;
    int incline;
    int resistance;
    FeState feState;
};

struct SpecificTrainerData {
    explicit SpecificTrainerData(const int updateEventCount, const int instantaneousCadence, FeState feState,
                                 const int instantaneousPower, const int accumulatedPower)
        : update_event_count(updateEventCount), instantaneous_cadence(instantaneousCadence),
          instantaneous_power(instantaneousPower), accumulated_power(accumulatedPower),
          feState(std::move(feState)) {
    }

    int update_event_count;
    int instantaneous_cadence;
    int instantaneous_power;
    int accumulated_power;
    FeState feState;
};
