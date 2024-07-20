#pragma once


class Measurement {
public:
    virtual ~Measurement() = default;
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

class FecMeasurement : public Measurement {
};

struct FeStateEvent final : FecMeasurement {
    FeStateEvent(std::string fe_state, bool lap_toggle)
        : fe_state(std::move(fe_state)), lap_toggle(lap_toggle) {
    }

    std::string fe_state; // Assuming fe_state is a string
    bool lap_toggle; // Assuming lap_toggle is a boolean
};

struct GeneralDataEvent final : FecMeasurement {
    GeneralDataEvent(int elapsed_time, int distance_traveled, int speed, int heart_rate, FeStateEvent fe_state_event)
        : elapsed_time(elapsed_time), distance_traveled(distance_traveled), speed(speed), heart_rate(heart_rate),
          fe_state_event(std::move(fe_state_event)) {
    }

    int elapsed_time;
    int distance_traveled;
    int speed;
    int heart_rate;
    FeStateEvent fe_state_event; // Assuming fe_state_event is a string
};

struct GeneralSettingsEvent final : FecMeasurement {
    GeneralSettingsEvent(int cycle_length, int incline, int resistance, FeStateEvent fe_state_event)
        : cycle_length(cycle_length), incline(incline), resistance(resistance),
          fe_state_event(std::move(fe_state_event)) {
    }

    int cycle_length;
    int incline;
    int resistance;
    FeStateEvent fe_state_event; // Assuming fe_state_event is a string
};

struct SpecificTrainerDataEvent {
    SpecificTrainerDataEvent(int update_event_count, int instantaneous_cadence, FeStateEvent fe_state_event,
                             int instantaneous_power, int accumulated_power)
        : update_event_count(update_event_count), instantaneous_cadence(instantaneous_cadence),
          fe_state_event(std::move(fe_state_event)), instantaneous_power(instantaneous_power),
          accumulated_power(accumulated_power) {
    }

    int update_event_count;
    int instantaneous_cadence;
    int instantaneous_power;
    int accumulated_power;
    FeStateEvent fe_state_event; // Assuming fe_state_event is a string
};
