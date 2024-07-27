#include "Controller.h"

#include <iostream>

std::ostream &operator<<(std::ostream &os, const State &state) {
    switch (state) {
        case State::STARTING: os << "STARTING";
            break;
        case State::WAITING_FOR_SENSORS: os << "WAITING_FOR_SENSORS";
            break;
        case State::WAITING_FOR_TRAINER: os << "WAITING_FOR_TRAINER";
            break;
        case State::IN_WORKOUT: os << "IN_WORKOUT";
            break;
        default: os << "EXITING";
            break;
    }
    return os;
}

void TrainerWindowController::handleRequest() {
    if (state->state != State::STARTING and state->state != State::WAITING_FOR_SENSORS) {
        std::cout << "  Wrong state: " << state->state << std::endl;
        return;
    }

    if (not history->empty()) {
        const auto previous = history->top();
        history->pop();

        std::cout << "  Hiding previous window of type: " << typeid(*previous).name() << std::endl;
        previous->hide();
    }

    std::cout << "  Moving window to x: " << state->x << " y: " << state->y << std::endl;
    view->move(state->x, state->y);
    std::cout << "  Showing window" << std::endl;
    view->show();
    std::cout << "  Setting focus" << std::endl;
    view->setFocus();

    history->push(view);
    state->state = State::WAITING_FOR_TRAINER;
    std::cout << "  Pushed window to history" << std::endl;
}

void SensorsWindowController::handleRequest() {
    if (state->state != State::WAITING_FOR_TRAINER) {
        std::cout << "  Wrong state: " << state->state << std::endl;
        return;
    }

    if (not history->empty()) {
        const auto previous = history->top();
        history->pop();
        previous->hide();
    }

    view->move(state->x, state->y);
    view->show();
    view->setFocus();

    state->state = State::WAITING_FOR_SENSORS;
    history->push(view);
}

void WorkoutWindowController::handleRequest() {
    if (state->state != State::WAITING_FOR_SENSORS) {
        std::cout << "  Wrong state: " << state->state << std::endl;
        return;
    }


    if (not history->empty()) {
        const auto previous = history->top();
        previous->hide();
    }

    view->move(state->x, state->y);
    view->show();
    view->setFocus();

    state->state = State::IN_WORKOUT;
    history->push(view);
}

void DeviceDialogController::handleRequest() {
    if (state->state != State::WAITING_FOR_SENSORS and state->state != State::WAITING_FOR_TRAINER) {
        std::cout << "  Wrong state: " << state->state << std::endl;
        return;
    }

    if (not history->empty()) {
        const auto previous = history->top();
        history->pop();

        QWidget &dialog = *previous;
        if (auto *d = dynamic_cast<DeviceDialog *>(&dialog)) {
            d->close();
            return;
        }

        previous->hide();
    }

    const auto window = std::make_shared<DeviceDialog>(nullptr);
    window->move(state->x, state->y);

    window->show();
    window->setFocus();

    history->push(window);
}
