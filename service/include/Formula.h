#pragma once

namespace BLE {
    namespace Math {
        constexpr auto INT_MATH_COEFFICIENT = 10;
        constexpr auto MM_IN_KM = 1000 * 1000;
        constexpr auto MS_IN_SECOND = 1024;
        constexpr auto MS_IN_MIN = 60 * 1024;
        constexpr auto MS_IN_HOUR = 60 * 60 * 1024;
    }

    namespace Wheels {
        constexpr auto DEFAULT_TIRE_CIRCUMFERENCE_MM = 2168;
    }
}
