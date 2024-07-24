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
          targetPowerLimits(std::move(targetPowerLimits)),
          instantaneousPower(instantaneousPower), accumulatedPower(accumulatedPower), trainerStatus(trainerStatus),
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

