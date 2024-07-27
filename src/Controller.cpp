#include "Controller.h"

#include <iostream>

std::ostream &operator<<(std::ostream &os, const ApplicationState &state) {
    switch (state) {
        case ApplicationState::STARTING: os << "STARTING";
            break;
        case ApplicationState::WAITING_FOR_SENSORS: os << "WAITING_FOR_SENSORS";
            break;
        case ApplicationState::WAITING_FOR_TRAINER: os << "WAITING_FOR_TRAINER";
            break;
        case ApplicationState::IN_WORKOUT: os << "IN_WORKOUT";
            break;
        default: os << "EXITING";
            break;
    }
    return os;
}

void TrainerWindowController::handleRequest() {
    if (state->state != ApplicationState::STARTING and state->state != ApplicationState::WAITING_FOR_SENSORS) {
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

    history->push(view);
    state->state = ApplicationState::WAITING_FOR_TRAINER;
}

void SensorsWindowController::handleRequest() {
    if (state->state != ApplicationState::WAITING_FOR_TRAINER) {
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

    state->state = ApplicationState::WAITING_FOR_SENSORS;
    history->push(view);
}

void WorkoutWindowController::handleRequest() {
    if (state->state != ApplicationState::WAITING_FOR_SENSORS) {
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

    state->state = ApplicationState::IN_WORKOUT;
    history->push(view);
}

void DeviceDialogController::handleRequest() {
    if (state->state != ApplicationState::WAITING_FOR_SENSORS and state->state != ApplicationState::WAITING_FOR_TRAINER) {
        std::cout << "  Wrong state: " << state->state << std::endl;
        return;
    }

    if (not history->empty()) {
        const auto previous = history->top();

        QWidget &window = *previous;
        if (auto *d = dynamic_cast<DeviceDialog *>(&window)) {
            if (d->selectedItem) {
                model->setDevice(d->selectedItem);
            }

            d->close();
            history->pop();
            return;
        }

        const auto dialog = std::make_shared<DeviceDialog>(&window);
        dialog->move(state->x, state->y);

        dialog->show();
        dialog->setFocus();

        history->push(dialog);
    }


}
