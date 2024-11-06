#include <QApplication>
#include <vector>
#include <spdlog/spdlog.h>

#include "ScannerService.h"

#include "Constants.h"
#include "Controller.h"
#include "QtEventPublisher.h"
#include "SystemTray.h"
#include "ViewNavigator.h"
#include "WorkoutWindow.h"

#include "BleDeviceServices.h"
#include "DeviceWindow.h"

using namespace winrt;

int main(int argc, char **argv) {
    winrt::init_apartment(apartment_type::multi_threaded);

    const auto app = new QApplication(argc, argv);
    const auto appState = std::make_shared<AppState>();
    const auto history = std::make_shared<std::stack<std::shared_ptr<QWidget> > >();

    const auto eventBus = std::make_shared<EventBus>();
    const auto model = std::make_shared<Model>(eventBus);

    const auto controllerHandler = std::make_shared<ControllerHandler>();
    const auto deviceWindow = std::make_shared<DeviceWindow>(controllerHandler);
    const auto selectWorkoutWindow = std::make_shared<SelectWorkoutWindow>(controllerHandler);
    const auto workoutWindow = std::make_shared<WorkoutWindow>(controllerHandler);
    const auto workoutSummaryWindow = std::make_shared<WorkoutSummaryWindow>(controllerHandler);
    const auto tray = std::make_shared<SystemTray>(controllerHandler);
    const auto deviceDialog = [&controllerHandler](const std::vector<std::shared_ptr<Device> > &data, QWidget *parent) {
        return std::make_shared<DeviceDialog>(data, controllerHandler, parent);
    };

    const auto qtAdapter = std::make_shared<QtEventPublisher>(deviceWindow, workoutWindow, workoutSummaryWindow, tray);

    eventBus->subscribe(EventType::DEVICE_DISCOVERED, [qtAdapter](const Event &event) {
        spdlog::trace("QtAdapter: DEVICE_DISCOVERED event reveived");

        const auto deviceDiscovered = dynamic_cast<const DeviceDiscovered *>(&event);
        assert(deviceDiscovered != nullptr && "deviceDiscovered should not be null");
        qtAdapter->deviceDiscovered(*deviceDiscovered);
    });
    eventBus->subscribe(EventType::DEVICE_SELECTED, [qtAdapter](const Event &event) {
        spdlog::trace("QtAdapter: DEVICE_SELECTED event reveived");

        const auto deviceSelected = dynamic_cast<const DeviceSelected *>(&event);
        assert(deviceSelected != nullptr && "deviceSelected should not be null");
        qtAdapter->deviceSelected(*deviceSelected);
    });
    eventBus->subscribe(EventType::SUBSCRIBED_TO_SERVICE, [qtAdapter](const Event &event) {
        spdlog::trace("QtAdapter: SUBSCRIBED_TO_SERVICE event reveived");

        const auto subscribedToService = dynamic_cast<const SubscribedToService *>(&event);
        assert(subscribedToService != nullptr && "subscribedToService should not be null");
        qtAdapter->subscribedToService(*subscribedToService);
    });
    eventBus->subscribe(EventType::WORKOUT_DATA, [qtAdapter](const Event &event) {
        spdlog::trace("QtAdapter: WORKOUT_DATA event reveived");

        const auto workoutEvent = dynamic_cast<const WorkoutEvent *>(&event);
        assert(workoutEvent != nullptr && "workoutEvent should not be null");

        qtAdapter->workoutData(*workoutEvent);
    });
    eventBus->subscribe(EventType::WORKOUT_DATA, [qtAdapter](const Event &event) {
        spdlog::trace("QtAdapter: WORKOUT_DATA event reveived");

        const auto workoutSummary = dynamic_cast<const WorkoutEvent *>(&event);
        assert(workoutSummary != nullptr && "workoutSummary should not be null");

        if (WorkoutState::IN_PROGRESS == workoutSummary->state) {
            return;
        }
        qtAdapter->workoutSummary(*workoutSummary);
    });
    eventBus->subscribe(EventType::DEVICE_CONNECTION, [controllerHandler](const Event &event) {
        spdlog::trace("DEVICE_CONNECTION event reveived");

        const auto bleConnectionEvent = dynamic_cast<const DeviceConnectionEvent *>(&event);
        assert(bleConnectionEvent != nullptr && "bleConnectionEvent should not be null");

        controllerHandler->next(Constants::States::DEVICE_STATUS_CHANGED, bleConnectionEvent);
    });


    const auto registry = std::make_shared<DeviceRegistry>(eventBus);
    const auto hrm = std::make_shared<HrmNotificationService>(registry, model);
    const auto csc = std::make_shared<CyclingCadenceAndSpeedNotificationService>(registry, model);
    const auto pwr = std::make_shared<PowerNotificationService>(registry, model);
    const auto fec = std::make_shared<FecService>(registry, model);
    const auto settingsManager = std::make_shared<SettingsManager>(model, eventBus, hrm, csc, pwr);
    const auto reconnector = std::make_shared<Reconnector>(model, hrm, csc, pwr);
    const auto workoutPacer = std::make_shared<WorkoutPacer>(model);
    const auto reconnectPacer = std::make_shared<ReconnectPacer>(reconnector);
    const auto scanner = std::make_shared<ScannerService>(model, Scanner(BLE::Services::SUPPORTED_SERVICES_MAP));

    const auto startupController = std::make_shared<StartupController>(settingsManager, controllerHandler, appState);
    const auto wheelSizeSelectionController = std::make_shared<WheelSizeSelectionController>(model);
    const auto speedUnitController = std::make_shared<SpeedUnitController>(model);
    const auto deviceWindowController = std::make_shared<DeviceWindowController>(
        deviceWindow, appState, history
    );
    const auto selectWorkoutWindowController = std::make_shared<SelectWorkoutWindowController>(
        scanner, selectWorkoutWindow, appState, history
    );
    const auto workoutWindowController = std::make_shared<WorkoutWindowController>(
        scanner, workoutWindow, model, workoutPacer, appState, history
    );
    const auto workoutSummaryWindowController = std::make_shared<WorkoutSummaryWindowController>(
        workoutSummaryWindow, model, workoutPacer, appState, history
    );
    const auto deviceDialogController = std::make_shared<DeviceDialogController>(
        qtAdapter, deviceDialog, appState, history, model
    );
    const auto switchThemeController = std::make_shared<SwitchThemeController>(
        app, deviceWindow, selectWorkoutWindow, workoutWindow, workoutSummaryWindow, appState, history
    );
    const auto shutdownController = std::make_shared<ShutdownController>(
        settingsManager,
        hrm, csc, pwr, fec, scanner, registry, appState
    );
    const auto connectToDeviceController = std::make_shared<ConnectToDeviceController>(
        hrm, csc, pwr, fec, appState
    );
    const auto deviceReconnectionController = std::make_shared<DeviceReconnectionController>(
        reconnector, reconnectPacer, appState
    );

    const auto viewNavigator = std::make_unique<ViewNavigator>(
        controllerHandler, startupController,
        deviceDialogController, connectToDeviceController, deviceWindowController,
        selectWorkoutWindowController, workoutWindowController, workoutSummaryWindowController, switchThemeController,
        shutdownController, wheelSizeSelectionController, speedUnitController, deviceReconnectionController
    );

    tray->switchTheme();
    tray->show();

    viewNavigator->nextScreen(Constants::Screens::STARTUP);

    return app->exec();
}
