#pragma once

// include/AppState.h
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
    AppState() : state(ApplicationState::STARTING), x(300), y(300) {
    }

    ApplicationState state;
    int x, y;
};
