#include "Controller.h"

#include <future>
#include <spdlog/spdlog.h>
#include <QApplication>
#include <QtConcurrent/QtConcurrent>

#include "BleDeviceServices.h"
#include "Constants.h"
#include "Reconnector.h"
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

auto StartupController::start() const -> void {
    QtConcurrent::run([this]() {
        initializer->initialise();
    });
    pacer->start();
    handler->next(Constants::Screens::SELECT_WORKOUT);
}

auto DeviceWindowController::showDeviceWindow() -> void {
    state->transitionTo(ApplicationState::WAITING_FOR_SENSORS);

    if (not history->empty()) {
        const auto previous = history->top();
        history->pop();
        previous->hide();
    }

    renderView();
    history->push(view);
}

auto SelectWorkoutWindowController::showSelectWorkoutWindow() -> void {
    state->transitionTo(ApplicationState::WAITING_FOR_WORKOUT);

    renderView();
    scannerService->startScan();
    history->push(view);
}

auto WorkoutWindowController::showWorkoutWindow() -> void {
    state->transitionTo(ApplicationState::IN_WORKOUT);

    scannerService->stopScan();
    model->startWorkout();
    pacer->start();
    renderView();
}

auto WorkoutSummaryWindowController::showWorkoutSummaryWindow() -> void {
    state->transitionTo(ApplicationState::WORKOUT_SUMMARY);

    model->stopWorkout();
    pacer->stop();
    renderView();
}

auto DeviceDialogController::showDialog() const -> void {
    if (!history->empty()) {
        const auto previous = history->top();

        QWidget &window = *previous;
        const auto devices = model->getDevices(nullptr);
        const auto dialog = createDialog(devices, &window);

        qtAdapter->setDeviceDialog(dialog);
        dialog->show();
        dialog->setFocus();

        history->push(dialog);
    }
}

auto DeviceDialogController::closeDialog() const -> void {
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
}

auto ConnectToDeviceController::connectToDevice(const std::shared_ptr<Device> &device) const -> void {
    if (state->state == ApplicationState::EXITING) {
        spdlog::info("  Wrong state: {}", state->state);
        return;
    }

    auto setupConnection = [this, device]() {
        if (device->services.contains(BLE::Services::HRM)) {
            hrmNotificationService->setDevice(device);
            settingsManager->store();
        }

        if (device->services.contains(BLE::Services::CSC)) {
            cscNotificationService->setDevice(device);
            settingsManager->store();
        }

        if (device->services.contains(BLE::Services::PWR)) {
            powerNotificationService->setDevice(device);
            settingsManager->store();
        }

        if (device->services.contains(BLE::Services::FEC_BIKE_TRAINER)) {
            fecService->setDevice(device);
            settingsManager->store();
        }
    };

    auto future = QtConcurrent::run(setupConnection);
}

auto SwitchThemeController::switchColorTheme() const -> void {
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


auto WheelSizeSelectionController::setWheelSize(WheelSize size) const -> void {
    model->setWheelSize(size);
    spdlog::info("Wheel size set to: {}", size);
}

void SpeedUnitController::setDistanceUnit(DistanceUnit size) const {
    model->setSpeedUnit(size);
    spdlog::info("Speed unit set to: {}", size);
}


auto DeviceReconnectionController::reconnect(const std::shared_ptr<Device> &device) const -> void {
    if (state->state == ApplicationState::EXITING) {
        spdlog::info("App shutdown in progress, ignoring connection loss for device [{}]", device->deviceId());
        return;
    }

    reconnector->registerDevice(device);
}

auto DeviceReconnectionController::connected(const std::shared_ptr<Device> &device) const -> void {
    if (state->state == ApplicationState::EXITING) {
        spdlog::info("App shutdown in progress, ignoring reconnect for device [{}]", device->deviceId());
        return;
    }

    reconnector->handleReconnect(device);
}

auto ShutdownController::shutdown() const -> void {
    state->transitionTo(ApplicationState::EXITING);

    initializer->store();
    pacer->stop();
    registry->stop();
    QApplication::quit();
}
