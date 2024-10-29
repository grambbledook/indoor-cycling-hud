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

auto DeviceWindowController::showDeviceWindow() -> void {
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

auto SelectWorkoutWindowController::showSelectWorkoutWindow() -> void {
    if (state->state != ApplicationState::WAITING_FOR_SENSORS && state->state != ApplicationState::WORKOUT_SUMMARY) {
        spdlog::info("  Wrong state: {}", state->state);
        return;
    }

    renderView();
    history->push(view);
    state->state = ApplicationState::WAITING_FOR_WORKOUT;
}

auto WorkoutWindowController::showWorkoutWindow() -> void {
    if (state->state != ApplicationState::WAITING_FOR_WORKOUT) {
        spdlog::info("  Wrong state: {}", state->state);
        return;
    }

    model->startWorkout();
    pacer->start();
    renderView();
    state->state = ApplicationState::IN_WORKOUT;
}

auto WorkoutSummaryWindowController::showWorkoutSummaryWindow() -> void {
    if (state->state != ApplicationState::IN_WORKOUT) {
        spdlog::info("  Wrong state: {}", state->state);
        return;
    }

    model->stopWorkout();
    pacer->stop();
    renderView();
    state->state = ApplicationState::WORKOUT_SUMMARY;
}

auto DeviceDialogController::showDialog() const -> void {
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

auto DeviceDialogController::closeDialog() const -> void {
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

    scannerService->stopScan();
}

auto ConnectToDeviceController::connectToDevice(const std::shared_ptr<Device> &device) const -> void {
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

DeviceReconnectionController::DeviceReconnectionController(
    const std::shared_ptr<DeviceRegistry> &registry,
    const std::shared_ptr<Model> &model,
    const std::shared_ptr<AppState> &state
): registry(registry), model(model), state(state) {
}

auto DeviceReconnectionController::reconnect(const std::shared_ptr<Device> device) const -> void {
    if (state->state == ApplicationState::EXITING) {
        spdlog::info("App shutdown in progress, ignoring connection loss for device [{}]", device->deviceId());
        return;
    }

    const auto services = model->getDeviceServices(device);
    if (services.empty()) {
        spdlog::info("  No services found for device: {}", device->deviceId());
        return;
    }

    spdlog::info("  Event received. device_id={}, status=DISCONNECTED", device->deviceId());
}

auto DeviceReconnectionController::connected(const std::shared_ptr<Device> device) const -> void {
    if (state->state == ApplicationState::EXITING) {
        spdlog::info("App shutdown in progress, ignoring reconnect for device [{}]", device->deviceId());
        return;
    }

    const auto services = model->getDeviceServices(device);
    if (services.empty()) {
        spdlog::info("  No services found for device: {}", device->deviceId());
        return;
    }

    spdlog::info("  Event received. device_id={}, status=CONNECTED", device->deviceId());
}

auto ShutdownController::shutdown() const -> void {
    state->state = ApplicationState::EXITING;

    registry->stop();
    QApplication::quit();
}
