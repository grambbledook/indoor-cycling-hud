#pragma once

struct Statistics {
    int latest;
    int min;
    int max;
    int average;
    int count;

    void aggregate(const int value) {
        latest = value;
        min = std::min(min, value);
        max = std::max(max, value);
        average = (average * count + value) / (count + 1);
        count++;
    }
};

struct CroppedStatistics {
    int val;
    int avg;
};

struct MeasurementsUpdate {
    CroppedStatistics hrm;
    CroppedStatistics cadence;
    CroppedStatistics speed;
    CroppedStatistics power;
};
