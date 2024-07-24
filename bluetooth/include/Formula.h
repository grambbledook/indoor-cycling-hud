#pragma once

namespace BLE {
    namespace Math {
        constexpr auto MM_TO_KM = 1.0 / 1000000;
        constexpr auto MS_TO_MIN = 1.0 / (60 * 1024);
        constexpr auto MS_TO_HOUR = 1.0 / (60 * 60 * 1024);
    }

    namespace Wheels {
        constexpr auto DEFAULT_TIRE_CIRCUMFERENCE_MM = 2168;
    }
}
