#include "Controller.h"

#include <future>
#include <spdlog/spdlog.h>
#include <QApplication>
#include <QtConcurrent/QtConcurrent>

#include "BleDeviceServices.h"
#include "StyleSheets.h"

template <>
struct fmt::formatter<ApplicationState> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const ApplicationState& state, FormatContext& ctx) {
        std::string stateStr;
        switch (state) {
            case ApplicationState::STARTING:
                stateStr = "STARTING";
            break;
            case ApplicationState::WAITING_FOR_TRAINER:
                stateStr = "WAITING_FOR_TRAINER";
            break;
            case ApplicationState::WAITING_FOR_SENSORS:
                stateStr = "WAITING_FOR_SENSORS";
            break;
            case ApplicationState::IN_WORKOUT:
                stateStr = "IN_WORKOUT";
            break;
            case ApplicationState::EXITING:
                stateStr = "EXITING";
            break;
        }
        return fmt::format_to(ctx.out(), "{}", stateStr);
    }
};

void TrainerWindowController::handleRequest() {
    if (state->state != ApplicationState::STARTING and state->state != ApplicationState::WAITING_FOR_SENSORS) {
        spdlog::info("  Wrong state: {}", state->state);
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
        spdlog::info("  Wrong state: {}", state->state);
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
        spdlog::info("  Wrong state: {}", state->state);
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
    if (state->state != ApplicationState::WAITING_FOR_SENSORS and state->state != ApplicationState::WAITING_FOR_TRAINER) {
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

void ConnectToDeviceController::handleRequest() {
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