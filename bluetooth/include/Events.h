#pragma once


class Measurement {
public:
    virtual ~Measurement() = default;
};

struct HrmMeasurement final : Measurement {
    explicit HrmMeasurement(const int hrm): Measurement(), hrm(hrm) {
    }

    unsigned int hrm;
};

struct SpeedMeasurement final : Measurement {
    SpeedMeasurement(const int cwr, const int lwet): Measurement(), cwr(cwr), lwet(lwet) {
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


template<typename T>
concept DerivedFromMeasurement = std::is_base_of<Measurement, T>::value;

template<DerivedFromMeasurement T>
class MeasurementEvent {
public:
    std::unique_ptr<Device> device{};
    T measurement;

    MeasurementEvent(std::unique_ptr<Device> device, T measurement)
        : device(std::move(device)), measurement(measurement) {
    }
};

//
//
// dataclass
// class FeStateEvent :
//         def __init__(self, fe_state, lap_toggle):
//         self.fe_state= fe_state
//         self.lap_toggle= lap_toggle
//
//
//         @ dataclass
//
// class GeneralDataEvent :
//         def __init__(self, elapsed_time, distance_traveled, speed, heart_rate, fe_state_event):
//         self.elapsed_time= elapsed_time
//         self.distance_traveled= distance_traveled
//         self.speed= speed
//         self.heart_rate= heart_rate
//         self.fe_state_event= fe_state_event
//
//
//         @ dataclass
//
// class GeneralSettingsEvent :
//         def __init__(self, cycle_length, incline, resistance, fe_state_event):
//         self.cycle_length= cycle_length
//         self.incline= incline
//         self.resistance= resistance
//         self.fe_state_event= fe_state_event
//
//
//         @ dataclass
//
// class SpecificTrainerDataEvent :
//         def __init__(self, update_event_count, instantaneous_cadence, fe_state_event, instantaneous_power,
//         accumulated_power):
//         self.update_event_count= update_event_count
//         self.instantaneous_cadence= instantaneous_cadence
//         self.fe_state_event= fe_state_event
//         self.instantaneous_power= instantaneous_power
//         self.accumulated_power= accumulated_power
