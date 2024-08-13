#include "Controller.h"

#include <future>
#include <iostream>
#include <QApplication>
#include <QtConcurrent/QtConcurrent>

#include "BleDeviceServices.h"
#include "StyleSheets.h"

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

void ShowDeviceDialogController::handleRequest() {
    if (state->state != ApplicationState::WAITING_FOR_SENSORS and state->state !=
        ApplicationState::WAITING_FOR_TRAINER) {
        std::cout << "  Wrong state: " << state->state << std::endl;
        return;
    }

    if (!history->empty()) {
        const auto previous = history->top();

        QWidget &window = *previous;
        const auto devices = model->getDevices(nullptr);
        const auto dialog = createDialog(devices, &window);

        qtAdapter->setDeviceDialog(dialog);
        dialog->show();
        dialog->setFocus();

        scannerService->startScan();
        history->push(dialog);
    }
}

void ConnectToDeviceController::handleRequest() {
    if (state->state == ApplicationState::EXITING) {
        std::cout << "  Wrong state: " << state->state << std::endl;
        return;
    }

    if (history->empty()) {
        std::cout << "  History stack is empty" << std::endl;
        return;
    }

    const auto previous = history->top();

    QWidget &window = *previous;
    const auto dialog = dynamic_cast<DeviceDialog *>(&window);
    if (!dialog) {
        std::cout << "  Wrong window type" << std::endl;
        return;
    }

    dialog->close();
    history->pop();

    const auto device = dialog->selectedItem;
    if (!device) {
        std::cout << "  No device selected" << std::endl;
        return;
    }

    scannerService->stopScan();

    auto setupConnection = [this, device]() {
        if (device->services.contains(BLE::Services::HRM)) {
            hrmNotificationService->setDevice(device);
        }

        if (device->services.contains(BLE::Services::CSC)) {
            cscNotificationService->setDevice(device);
        }

        if (device->services.contains(BLE::Services::PWR)) {
            powerNotificationService->setDevice(device);
        }

        if (device->services.contains(BLE::Services::FEC_BIKE_TRAINER)) {
            fecService->setDevice(device);
        }
    };

    auto future = QtConcurrent::run(setupConnection);
}

void SwitchThemeController::handleRequest() {
    state->darkThemeEnabled = !state->darkThemeEnabled;

    const auto theme = state->darkThemeEnabled ? StyleSheets::THEME_DARK : StyleSheets::THEME_BRIGHT;
    const auto size = StyleSheets::SCALE_MEDIUM;

    const auto sheet = QString::fromStdString(theme + size);

    app->setStyleSheet(sheet);

    if (!history->empty()) {
        const auto possiblyDialog = history->top();
        possiblyDialog->setStyleSheet(sheet);
        possiblyDialog->update();
    }
    workoutWindow->setStyleSheet(sheet);
    sensorsWindow->setStyleSheet(sheet);
    trainerWindow->setStyleSheet(sheet);
    workoutWindow->update();
    sensorsWindow->update();
    trainerWindow->update();

}

void ShutdownController::handleRequest() {
    state->state = ApplicationState::EXITING;

    registry->stop();
    QApplication::quit();
}
