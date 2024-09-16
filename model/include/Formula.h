#pragma once
#include "Units.h"

namespace BLE {
    namespace Math {
        constexpr auto INT_MATH_COEFFICIENT = 100;
        constexpr auto MM_IN_KM = 1000 * 1000;
        constexpr auto MS_IN_SECOND = 1000;
        constexpr auto SECONDS_IN_MINUTE = 60;
        constexpr auto BLE_MS_IN_SECOND = 1024;
        constexpr auto MS_IN_MIN = 60 * 1000;
        constexpr auto MS_IN_HOUR = 60 * 60 * 1000;

        inline long computeCadence(
            const long lcet, const long prevLcet, const long long ccr, const long long prevCcr
        ) {

            const auto lcetResetCorrection = lcet > prevLcet ? 0 : 0x10000;
            const auto ccrResetCorrection = ccr > prevCcr ? 0 : 0xFFFFFFFF;

            const auto totalRevolutions = ccr - prevCcr + ccrResetCorrection;
            const auto timeDelta = lcet - prevLcet + lcetResetCorrection;

            // for lwet unit is 1 / 1024 seconds
            const auto timeDeltaMS = timeDelta * MS_IN_SECOND / BLE_MS_IN_SECOND;

            const auto cadence = totalRevolutions * MS_IN_MIN / timeDeltaMS;
            return cadence;
        }

        inline long computeSpeed(
            const long lwet, const long prevLwet, const long cwr, const long prevCwr,
            const long wheelCircumferenceInMM
        ) {

            // Because the counter can go down and given that max uint32
            // combined with 2.1m wheel size gives un roughly 9 million kilometers in total,
            // which is unlikely to happen during a workout, we don't do correction for CWR
            const auto lwetResetCorrection = lwet > prevLwet ? 0 : 0x10000;

            const auto totalRevolutions = cwr - prevCwr;
            const auto timeDelta = lwet - prevLwet + lwetResetCorrection;

            // for lwet unit is 1 / 1024 seconds
            const auto timeDeltaMS = timeDelta * MS_IN_SECOND / BLE_MS_IN_SECOND;
            const auto distanceTraveled = totalRevolutions * wheelCircumferenceInMM;
            const auto speedMms = distanceTraveled * INT_MATH_COEFFICIENT / timeDeltaMS;

            return speedMms;
        }

        // Returns total distance in meters or in miles times 1000
        inline long computeDistance(
            const long speed, const long long duration, DistanceUnit distanceUnit
        ) {
            return speed * duration * getDistanceConversionFactor(distanceUnit);
        }
    }
}
