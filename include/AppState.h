#pragma once

// include/AppState.h
#pragma once

enum class State {
    STARTING,
    WAITING_FOR_TRAINER,
    WAITING_FOR_SENSORS,
    IN_WORKOUT,
    EXITING
};



class AppState {
public:
    AppState() : state(State::STARTING), x(300), y(300) {
    }

    State state;
    int x, y;
};
