#pragma once
#include "Events.h"

template<typename T, typename = std::enable_if_t<std::is_same_v<T, int> || std::is_same_v<T, double>> >
struct Statistics {
    T latest;
    T min;
    T max;
    double average;
    int count;

    void aggregate(T value) {
        latest = value;
        min = std::min(min, value);
        max = std::max(max, value);
        average = (average * count + value) / (count + 1);
        count++;
    }
};


template<typename A, typename B, typename = std::enable_if_t<std::is_same_v<B, int> || std::is_same_v<B, double>> >
struct State {
    std::shared_ptr<Device> device;
    std::vector<A> data;
    Statistics<B> stats;

    std::vector<A> getLastN(int n) {
        if (n > data.size()) {
            return std::vector<A>(n);
        }

        return std::vector<A>(data.end() - n, data.end());
    }

    void recordMetric(A value) {
        if (data.size() == 2) {
            data.erase(data.begin());
        }
        data.push_back(value);
    }

    void unrecordMetric() {
        data.erase(data.end());
    };

    void aggregateMetric(B value) {
        stats.aggregate(value);
    }
};


class Model {
public:
    template<DerivedFromMeasurement T>
    void recordData(MeasurementEvent<T> event);

private:
    State<int, int> hrmState = {
        __nullptr,
        std::vector<int>{},
        {0, 0, 0, 0.0, 0}
    };

    State<std::pair<int, int>, int> cadenceState = {
        __nullptr,
        std::vector<std::pair<int, int> >{},
        {0, 0, 0, 0.0, 0}
    };

    State<std::pair<int, int>, double> speedState = {
        __nullptr,
        std::vector<std::pair<int, int> >{},
        {0.0, 0.0, 0.0, 0.0, 0}
    };

    State<int, int> powerState = {
        __nullptr,
        std::vector<int>{},
        {0, 0, 0, 0.0, 0}
    };
};
