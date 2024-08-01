#pragma once

enum class ApplicationState {
    STARTING,
    WAITING_FOR_TRAINER,
    WAITING_FOR_SENSORS,
    IN_WORKOUT,
    EXITING
};

class AppState {
public:
    AppState() : state(ApplicationState::STARTING) {
    }

    ApplicationState state;
};
