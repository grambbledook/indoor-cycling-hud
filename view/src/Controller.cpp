#include "Controller.h"

#include <future>
#include <spdlog/spdlog.h>
#include <QApplication>
#include <QtConcurrent/QtConcurrent>

#include "BleDeviceServices.h"
#include "StyleSheets.h"


template<typename T, typename T0>
auto ViewController<T, T0>::renderView() -> void {
    auto x = 300;
    auto y = 300;
    if (!this->history->empty()) {
        const auto previous = this->history->top();
        this->history->pop();
        previous->hide();

        x = previous->x();
        y = previous->y();
    }

    view->move(x, y);

    view->show();
    view->setFocus();

    this->history->push(view);
}

auto UeberWindowController::handleRequest(void *) -> void {
    if (state->state != ApplicationState::STARTING) {
        spdlog::info("  Wrong state: {}", state->state);
        return;
    }

    if (not history->empty()) {
        const auto previous = history->top();
        history->pop();
        previous->hide();
    }

    renderView();
    history->push(view);
    state->state = ApplicationState::WAITING_FOR_SENSORS;
}

auto SelectWorkoutWindowController::handleRequest(void *) -> void {
    if (state->state != ApplicationState::WAITING_FOR_SENSORS && state->state != ApplicationState::WORKOUT_SUMMARY) {
        spdlog::info("  Wrong state: {}", state->state);
        return;
    }

    renderView();
    history->push(view);
    state->state = ApplicationState::WAITING_FOR_WORKOUT;
}

auto WorkoutWindowController::handleRequest(void *) -> void {
    if (state->state != ApplicationState::WAITING_FOR_WORKOUT) {
        spdlog::info("  Wrong state: {}", state->state);
        return;
    }

    model->startWorkout();
    pacer->start();
    renderView();
    state->state = ApplicationState::IN_WORKOUT;
}

auto WorkoutSummaryWindowController::handleRequest(void *) -> void {
    if (state->state != ApplicationState::IN_WORKOUT) {
        spdlog::info("  Wrong state: {}", state->state);
        return;
    }

    model->stopWorkout();
    pacer->stop();
    renderView();
    state->state = ApplicationState::WORKOUT_SUMMARY;
}

auto ShowDeviceDialogController::handleRequest(void *) -> void {
    if (state->state != ApplicationState::WAITING_FOR_SENSORS) {
        spdlog::info("  Wrong state: {}", state->state);
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

auto ConnectToDeviceController::handleRequest(void *) -> void {
    if (state->state == ApplicationState::EXITING) {
        spdlog::info("  Wrong state: {}", state->state);
        return;
    }

    if (history->empty()) {
        spdlog::info("  History stack is empty");
        return;
    }

    const auto previous = history->top();

    QWidget &window = *previous;
    const auto dialog = dynamic_cast<DeviceDialog *>(&window);
    if (!dialog) {
        spdlog::info("  Wrong window type");
        return;
    }

    dialog->close();
    history->pop();

    const auto device = dialog->selectedItem;
    if (!device) {
        spdlog::info("  No device selected");
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

auto TrayConnectToDeviceController::handleRequest(const std::shared_ptr<Device> &device) -> void {
    if (state->state == ApplicationState::EXITING) {
        spdlog::info("  Wrong state: {}", state->state);
        return;
    }

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

auto SwitchThemeController::handleRequest(void *) -> void {
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
    selectWorkoutWindow->setStyleSheet(sheet);
    deviceWindow->setStyleSheet(sheet);
    workoutSummaryWindow->setStyleSheet(sheet);
    workoutWindow->update();
    selectWorkoutWindow->update();
    deviceWindow->update();
    workoutSummaryWindow->update();
}

auto ShutdownController::handleRequest(void *) -> void {
    state->state = ApplicationState::EXITING;

    registry->stop();
    QApplication::quit();
}

auto WheelSizeSelectionController::handleRequest(WheelSize size) -> void {
    model->setWheelSize(size);
    spdlog::info("Wheel size set to: {}", size);
}

auto SpeedUnitController::handleRequest(DistanceUnit size) -> void {
    model->setSpeedUnit(size);
    spdlog::info("Speed unit set to: {}", size);
}
