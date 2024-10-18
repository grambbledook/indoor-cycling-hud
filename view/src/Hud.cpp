#include <QApplication>
#include <vector>

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
    auto appState = std::make_shared<AppState>();
    auto history = std::make_shared<std::stack<std::shared_ptr<QWidget> > >();

    auto controllerHandler = std::make_shared<ControllerHandler>();

    auto deviceDialog = [&controllerHandler](std::vector<std::shared_ptr<Device> > data, QWidget *parent) {
        return std::make_shared<DeviceDialog>(data, controllerHandler, parent);
    };

    auto model = std::make_shared<Model>();

    auto deviceWindow = std::make_shared<DeviceWindow>(controllerHandler);
    auto deviceWindowController = std::make_shared<UeberWindowController>(
        deviceWindow, appState, history);

    auto selectWorkoutWindow = std::make_shared<SelectWorkoutWindow>(controllerHandler);
    auto selectWorkoutWindowController = std::make_shared<SelectWorkoutWindowController>(
        selectWorkoutWindow, appState, history);

    auto pacer = std::make_shared<WorkoutPacer>(model);

    auto workoutWindow = std::make_shared<WorkoutWindow>(controllerHandler);
    auto workoutWindowController = std::make_shared<WorkoutWindowController>(
        workoutWindow, model, pacer, appState, history);

    auto workoutSummaryWindow = std::make_shared<WorkoutSummaryWindow>(controllerHandler);
    auto workoutSummaryWindowController = std::make_shared<WorkoutSummaryWindowController>(
        workoutSummaryWindow, model, pacer, appState, history);

    const auto tray = std::make_shared<SystemTray>(controllerHandler);

    auto qtAdapter = std::make_shared<QtEventPublisher>(deviceWindow, workoutWindow, workoutSummaryWindow, tray);

    model->notifications.deviceDiscovered.subscribe(
        std::bind(&QtEventPublisher::deviceDiscovered, qtAdapter, std::placeholders::_1));
    model->notifications.deviceSelected.subscribe(
        std::bind(&QtEventPublisher::deviceSelected, qtAdapter, std::placeholders::_1));
    model->notifications.measurements.subscribe(
        std::bind(&QtEventPublisher::workoutData, qtAdapter, std::placeholders::_1));
    model->notifications.summary.subscribe(
        std::bind(&QtEventPublisher::workoutSummary, qtAdapter, std::placeholders::_1));

    auto scanner1 = Scanner(BLE::Services::SUPPORTED_SERVICES_MAP);
    auto scanner = std::make_shared<ScannerService>(model, scanner1);

    auto deviceDialogController = std::make_shared<ShowDeviceDialogController>(
        qtAdapter, scanner, deviceDialog, appState, history, model);

    auto registry = std::make_shared<DeviceRegistry>();
    const auto hrm = std::make_shared<HrmNotificationService>(registry, model);
    const auto csc = std::make_shared<CyclingCadenceAndSpeedNotificationService>(registry, model);
    const auto pwr = std::make_shared<PowerNotificationService>(registry, model);
    const auto fec = std::make_shared<FecService>(registry, model);
    auto connectToDeviceController = std::make_shared<ConnectToDeviceController>(
        hrm, csc, pwr, fec, scanner, appState, history);

    auto switchThemeController = std::make_shared<SwitchThemeController>(
        app, deviceWindow, selectWorkoutWindow, workoutWindow, workoutSummaryWindow, appState, history
    );

    const auto shutdownController = std::make_shared<ShutdownController>(
        hrm, csc, pwr, fec, scanner, registry, appState);

    const auto wheelSizeSelectionController = std::make_shared<WheelSizeSelectionController>(model);

    const auto speedUnitController = std::make_shared<SpeedUnitController>(model);

    auto trayConnectToDeviceController = std::make_shared<TrayConnectToDeviceController>(
        hrm, csc, pwr, fec, appState);

    const auto viewNavigator = std::make_unique<ViewNavigator>(
        controllerHandler,
        deviceDialogController, connectToDeviceController, deviceWindowController,
        selectWorkoutWindowController, workoutWindowController, workoutSummaryWindowController, switchThemeController,
        shutdownController, wheelSizeSelectionController, speedUnitController, trayConnectToDeviceController
    );

    tray->switchTheme();
    tray->show();

    viewNavigator->nextScreen(Constants::Screens::SENSORS);

    return app->exec();
}
