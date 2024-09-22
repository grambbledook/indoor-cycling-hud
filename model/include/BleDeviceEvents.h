#pragma once

#include <memory>
#include <string>
#include <type_traits>

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

    unsigned long hrm;
};

struct SpeedMeasurement final : Measurement {
    explicit SpeedMeasurement(const int cwr, const int lwet): Measurement(), cwr(cwr), lwet(lwet) {
    }

    unsigned long cwr;
    unsigned long lwet;
};

struct CadenceMeasurement final : Measurement {
    CadenceMeasurement(const int ccr, const int lcet): Measurement(), ccr(ccr), lcet(lcet) {
    }

    unsigned long ccr;
    unsigned long lcet;
};

struct PowerMeasurement final : Measurement {
    explicit PowerMeasurement(const int power): Measurement(), power(power) {
    }

    unsigned long power;
};

class FecMeasurement : public Measurement {
};

struct FeState final : FecMeasurement {
    explicit FeState(std::string feState, const bool lapToggle)
        : state(std::move(feState)), lapToggle(lapToggle) {
    }

    std::string state;
    bool lapToggle;
};

struct GeneralData final : FecMeasurement {
    explicit GeneralData(const int elapsedTime, const int distanceTraveled, const int speed, const int heartRate,
                         FeState feState)
        : elapsedTime(elapsedTime), distanceTraveled(distanceTraveled), speed(speed), heartRate(heartRate),
          feState(std::move(feState)) {
    }

    int elapsedTime;
    int distanceTraveled;
    int speed;
    int heartRate;
    FeState feState;
};

struct GeneralSettings final : FecMeasurement {
    explicit GeneralSettings(const int cycleLength, const int incline, const int resistance, FeState feState)
        : cycleLength(cycleLength), incline(incline), resistance(resistance),
          feState(std::move(feState)) {
    }

    int cycleLength;
    int incline;
    int resistance;
    FeState feState;
};

struct TrainerStatus {
    bool powerCalibrationRequired;
    bool resistanceCalibrationRequired;
    bool userConfigurationRequired;
};

struct SpecificTrainerData : FecMeasurement {
    explicit SpecificTrainerData(const int updateEventCount, const int instantaneousCadence,
                                 const int instantaneousPower, const int accumulatedPower,
                                 std::string targetPowerLimits, const TrainerStatus trainerStatus, FeState feState)
        : updateEventCount(updateEventCount), instantaneousCadence(instantaneousCadence),
          instantaneousPower(instantaneousPower),
          accumulatedPower(accumulatedPower), targetPowerLimits(std::move(targetPowerLimits)), trainerStatus(trainerStatus),
          feState(std::move(feState)) {
    }

    int updateEventCount;
    int instantaneousCadence;
    int instantaneousPower;
    int accumulatedPower;
    std::string targetPowerLimits;
    TrainerStatus trainerStatus;
    FeState feState;
};

