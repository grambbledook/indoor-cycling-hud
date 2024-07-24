#pragma once


namespace BLE::Tacx {
    namespace Power {
        const auto IN_RANGE = "OK";
        const auto TOO_LOW = "TOO_LOW";
        const auto TOO_HIGH = "TOO_HIGH";
        const auto UNKNOWN = "UNKNOWN";
    }

    namespace Status {
        const auto ASLEEP = "ASLEEP";
        const auto READY = "READY";
        const auto IN_USE = "IN_USE";
        const auto FINISHED = "FINISHED";
        const auto UNKNOWN = "UNKNOWN";
    }
}
