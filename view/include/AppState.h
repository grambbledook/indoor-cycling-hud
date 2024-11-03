#pragma once
#include <fmt/core.h>


enum class ApplicationState {
    STARTING,
    WAITING_FOR_SENSORS,
    WAITING_FOR_WORKOUT,
    IN_WORKOUT,
    WORKOUT_SUMMARY,
    EXITING
};

inline auto toString(const ApplicationState state) -> std::string {
    switch (state) {
        case ApplicationState::STARTING:
            return "STARTING";
        case ApplicationState::WAITING_FOR_SENSORS:
            return "WAITING_FOR_SENSORS";
        case ApplicationState::IN_WORKOUT:
            return "IN_WORKOUT";
        case ApplicationState::EXITING:
            return "EXITING";
        default:
            std::unreachable();
    }
}

template<>
struct fmt::formatter<ApplicationState> {
    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const ApplicationState &state, FormatContext &ctx) {
        return fmt::format_to(ctx.out(), "{}", toString(state));
    }
};

class AppState {
public:
    AppState() : state(ApplicationState::STARTING) {
    }

    ApplicationState state;
    bool darkThemeEnabled = false;

    auto transitionTo(const ApplicationState &to) -> void {
        assert(isTransitionAllowed(to)&& "Transition not allowed");
        state = to;
    }

    [[nodiscard]] auto isTransitionAllowed(const ApplicationState &to) const -> bool {
        switch (state) {
            case ApplicationState::STARTING:
                return false
                       || to == ApplicationState::WAITING_FOR_WORKOUT
                       || to == ApplicationState::EXITING;
            case ApplicationState::WAITING_FOR_WORKOUT:
                return false
                       || to == ApplicationState::WAITING_FOR_SENSORS
                       || to == ApplicationState::IN_WORKOUT
                       || to == ApplicationState::EXITING;
            case ApplicationState::WAITING_FOR_SENSORS:
                return false
                       || to == ApplicationState::WAITING_FOR_WORKOUT
                       || to == ApplicationState::IN_WORKOUT
                       || to == ApplicationState::EXITING;
            case ApplicationState::IN_WORKOUT:
                return false
                       || to == ApplicationState::WORKOUT_SUMMARY
                       || to == ApplicationState::EXITING;
            case ApplicationState::WORKOUT_SUMMARY:
                return false
                       || to == ApplicationState::WAITING_FOR_WORKOUT
                       || to == ApplicationState::EXITING;
            default:
                return false;
        };
    };
};
