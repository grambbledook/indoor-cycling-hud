#pragma once
#include <fmt/core.h>

enum class ApplicationState {
    STARTING,
    WAITING_FOR_TRAINER,
    WAITING_FOR_SENSORS,
    WAITING_FOR_WORKOUT,
    IN_WORKOUT,
    WORKOUT_SUMMARY,
    EXITING
};

template<>
struct fmt::formatter<ApplicationState> {
    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const ApplicationState &state, FormatContext &ctx) {
        std::string stateStr;
        switch (state) {
            case ApplicationState::STARTING:
                stateStr = "STARTING";
            break;
            case ApplicationState::WAITING_FOR_TRAINER:
                stateStr = "WAITING_FOR_TRAINER";
            break;
            case ApplicationState::WAITING_FOR_SENSORS:
                stateStr = "WAITING_FOR_SENSORS";
            break;
            case ApplicationState::IN_WORKOUT:
                stateStr = "IN_WORKOUT";
            break;
            case ApplicationState::EXITING:
                stateStr = "EXITING";
            break;
        }
        return fmt::format_to(ctx.out(), "{}", stateStr);
    }
};


class AppState {
public:
    AppState() : state(ApplicationState::STARTING) {
    }

    ApplicationState state;
    bool darkThemeEnabled = false;
};
