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

    auto x = 300;
    auto y = 300;
    if (not history->empty()) {
        const auto previous = history->top();
        history->pop();
        x = previous->x();
        y = previous->y();
        previous->hide();
    }

    view->move(x, y);
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

    auto x = 300;
    auto y = 300;
    if (not history->empty()) {
        const auto previous = history->top();
        history->pop();
        x = previous->x();
        y = previous->y();
        previous->hide();
    }

    view->move(x, y);
    view->show();
    view->setFocus();

    history->push(view);
    state->state = ApplicationState::WAITING_FOR_SENSORS;
}

void WorkoutWindowController::handleRequest() {
    if (state->state != ApplicationState::WAITING_FOR_SENSORS) {
        std::cout << "  Wrong state: " << state->state << std::endl;
        return;
    }

    auto x = 300;
    auto y = 300;
    if (not history->empty()) {
        const auto previous = history->top();
        history->pop();
        x = previous->x();
        y = previous->y();
        previous->hide();
    }

    view->move(x, y);
    view->show();
    view->setFocus();

    history->push(view);
    state->state = ApplicationState::IN_WORKOUT;
}

void DeviceDialogController::handleRequest() {
    if (state->state != ApplicationState::WAITING_FOR_SENSORS and state->state !=
        ApplicationState::WAITING_FOR_TRAINER) {
        std::cout << "  Wrong state: " << state->state << std::endl;
        return;
    }

    if (not history->empty()) {
        const auto previous = history->top();

        QWidget &window = *previous;
        if (auto *d = dynamic_cast<DeviceDialog *>(&window)) {
            if (d->selectedItem) {
                std::cout << "  Device selected: " << d->selectedItem->name.value << std::endl;
                model->setDevice(d->selectedItem);
            }

            d->close();
            history->pop();
            scanneService->stopScan();
            return;
        }

        const auto devices = model->getDevices(nullptr);

        const auto dialog = createDialog(devices, &window);

        qtAdapter->setDeviceDialog(dialog);
        dialog->show();
        dialog->setFocus();

        scanneService->startScan();
        history->push(dialog);
    }
}
