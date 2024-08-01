#include <QApplication>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QtPlugin>
#include <vector>

#include "ScannerService.h"

#include "Constants.h"
#include "Controller.h"
#include "QtEventPublisher.h"
#include "SensorsWindow.h"
#include "TrainerWindow.h"
#include "ViewNavigator.h"
#include "WorkoutWindow.h"

#include "WinRT.h"

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
    auto trainerWindow = std::make_shared<TrainerWindow>(controllerHandler);
    auto trainerWindowController = std::make_shared<TrainerWindowController>(
        trainerWindow, appState, history);

    auto sensorsWindow = std::make_shared<SensorsWindow>(controllerHandler);
    auto sensorWindowController = std::make_shared<SensorsWindowController>(
        sensorsWindow, appState, history);

    auto workoutWindow = std::make_shared<WorkoutWindow>(controllerHandler);
    auto workoutWindowController = std::make_shared<WorkoutWindowController>(
        workoutWindow, appState, history);

    auto qtAdapter = std::make_shared<QtEventPublisher>(
        trainerWindow, sensorsWindow, workoutWindow
    );

    model->notifications.deviceDiscovered.subscribe(
        std::bind(&QtEventPublisher::deviceDiscovered, qtAdapter, std::placeholders::_1));
    model->notifications.deviceSelected.subscribe(
        std::bind(&QtEventPublisher::deviceSelected, qtAdapter, std::placeholders::_1));
    model->notifications.measurements.subscribe(
        std::bind(&QtEventPublisher::measurementReceived, qtAdapter, std::placeholders::_1));

    auto scanner = std::make_shared<ScannerService>(model, Scanner());

    auto deviceDialogController = std::make_shared<ShowDeviceDialogController>(
        qtAdapter, scanner, deviceDialog, appState, history, model);


    auto registry = std::make_shared<DeviceRegistry>();
    const auto hrm = std::make_shared<HrmNotificationService>(registry, model);
    const auto csc = std::make_shared<CyclingCadenceAndSpeedNotificationService>(registry, model);
    const auto pwr = std::make_shared<PowerNotificationService>(registry, model);
    const auto fec = std::make_shared<FecService>(registry, model);
    auto connectToDeviceController = std::make_shared<ConnectToDeviceController>(
        hrm, csc, pwr, fec, scanner, appState, history);

    const auto shutdownController = std::make_shared<ShutdownController>(
        hrm, csc, pwr, fec, scanner, registry, appState);

    auto viewNavigator = std::make_unique<ViewNavigator>(
        controllerHandler,
        deviceDialogController, connectToDeviceController, trainerWindowController, sensorWindowController,
        workoutWindowController, shutdownController
    );

    viewNavigator->nextScreen(Constants::Screens::TRAINER);
    return app->exec();
}
