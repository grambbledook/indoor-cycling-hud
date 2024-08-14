#pragma once

enum class ApplicationState {
    STARTING,
    WAITING_FOR_TRAINER,
    WAITING_FOR_SENSORS,
    WAITING_FOR_WORKOUT,
    IN_WORKOUT,
    WORKOUT_SUMMARY,
    EXITING
};

class AppState {
public:
    AppState() : state(ApplicationState::STARTING) {
    }

    ApplicationState state;
    bool darkThemeEnabled = false;
};
