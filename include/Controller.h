#pragma once
#include <QWidget>
#include <type_traits>
#include <utility>
#include <stack>
#include <memory>
#include <QApplication>

#include "AppState.h"
#include "DeviceDialog.h"
#include "Model.h"
#include "NotificationService.h"
#include "QtEventPublisher.h"
#include "ScannerService.h"
#include "SensorsWindow.h"
#include "TrainerWindow.h"
#include "WorkoutWindow.h"


template<typename T, typename = std::enable_if_t<std::is_base_of_v<QWidget, T> > >
class Controller {
public:
    virtual ~Controller() = default;

    explicit Controller(
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history)
        : history(history), state(state) {
    }

    virtual void handleRequest() = 0;

    void print(std::string mod) {
        if (history->empty()) {
            std::cout << "[" << mod << "] History is empty" << std::endl;
        } else {
            auto previous = history->top();
            std::cout << "Controller [" << mod << "]: " << typeid(*previous).name() << std::endl;
        }
    }

protected:
    std::shared_ptr<Model> model;
    std::shared_ptr<AppState> state;
    std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > history;
};

template<typename T, typename = std::enable_if_t<std::is_base_of_v<QWidget, T> > >
class ViewController : public Controller<T> {
public:
    explicit ViewController(
        const std::shared_ptr<T> view,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history
    )
        : Controller<T>(state, history), view(view) {
    }

public:
    std::shared_ptr<T> view;
};

class TrainerWindowController final : public ViewController<TrainerWindow> {
public:
    explicit TrainerWindowController(
        const std::shared_ptr<TrainerWindow> &view,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history
    )
        : ViewController(view, state, history) {
    }

    void handleRequest() override;
};

class SensorsWindowController final : public ViewController<SensorsWindow> {
public:
    explicit SensorsWindowController(
        const std::shared_ptr<SensorsWindow> &view,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history
    )
        : ViewController(view, state, history) {
    }

    void handleRequest() override;
};

class WorkoutWindowController final : public ViewController<WorkoutWindow> {
public:
    explicit WorkoutWindowController(
        const std::shared_ptr<WorkoutWindow> &view,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history
    )
        : ViewController(view, state, history) {
    }

    void handleRequest() override;
};

class ShowDeviceDialogController final : public Controller<DeviceDialog> {
public:
    explicit ShowDeviceDialogController(
        const std::shared_ptr<QtEventPublisher> &qtAdapter,
        const std::shared_ptr<ScannerService> &scannerService,
        const std::function<std::shared_ptr<DeviceDialog>(std::vector<std::shared_ptr<Device> >, QWidget *)> &
        createDialog,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history,
        const std::shared_ptr<Model> &model
    )
        : Controller(state, history), createDialog(createDialog), model(model), scannerService(scannerService),
          qtAdapter(qtAdapter) {
    }

    void handleRequest() override;

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<ScannerService> scannerService;
    std::function<std::shared_ptr<DeviceDialog>(std::vector<std::shared_ptr<Device> >, QWidget *)> createDialog;
    std::shared_ptr<QtEventPublisher> qtAdapter;
};

class ConnectToDeviceController final : public Controller<DeviceDialog> {
public:
    explicit ConnectToDeviceController(
        const std::shared_ptr<HrmNotificationService> &hrmNotificationService,
        const std::shared_ptr<CyclingCadenceAndSpeedNotificationService> &cscNotificationService,
        const std::shared_ptr<PowerNotificationService> &powerNotificationService,
        const std::shared_ptr<FecService> &fecService,
        const std::shared_ptr<ScannerService> &scannerService,

        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history
    )
        : Controller(state, history), hrmNotificationService(hrmNotificationService),
          cscNotificationService(cscNotificationService),
          powerNotificationService(powerNotificationService),
          fecService(fecService), scannerService(scannerService) {
    }

    void handleRequest() override;

private:
    std::shared_ptr<HrmNotificationService> hrmNotificationService;
    std::shared_ptr<CyclingCadenceAndSpeedNotificationService> cscNotificationService;
    std::shared_ptr<PowerNotificationService> powerNotificationService;
    std::shared_ptr<FecService> fecService;
    std::shared_ptr<ScannerService> scannerService;
};

class SwitchThemeController final : public Controller<QWidget> {
public:
    explicit SwitchThemeController(
        QApplication *app,
        const std::shared_ptr<TrainerWindow> &trainerWindow,
        const std::shared_ptr<SensorsWindow> &sensorsWindow,
        const std::shared_ptr<WorkoutWindow> &workoutWindow,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history
    )
        : Controller(state, history), trainerWindow(trainerWindow), sensorsWindow(sensorsWindow),
          workoutWindow(workoutWindow) {
    }

    void handleRequest() override;

private:
    QApplication *app;
    std::shared_ptr<TrainerWindow> trainerWindow;
    std::shared_ptr<SensorsWindow> sensorsWindow;
    std::shared_ptr<WorkoutWindow> workoutWindow;
};

class ShutdownController final : public Controller<DeviceDialog> {
public:
    explicit ShutdownController(
        const std::shared_ptr<HrmNotificationService> &hrmNotificationService,
        const std::shared_ptr<CyclingCadenceAndSpeedNotificationService> &cscNotificationService,
        const std::shared_ptr<PowerNotificationService> &powerNotificationService,
        const std::shared_ptr<FecService> &fecService,
        const std::shared_ptr<ScannerService> &scannerService,
        const std::shared_ptr<DeviceRegistry> &registry,
        const std::shared_ptr<AppState> &state
    )
        : Controller(state, history), hrmNotificationService(hrmNotificationService),
          cscNotificationService(cscNotificationService), powerNotificationService(powerNotificationService),
          fecService(fecService), scannerService(scannerService), registry(registry) {
    }

    void handleRequest() override;

private:
    std::shared_ptr<HrmNotificationService> hrmNotificationService;
    std::shared_ptr<CyclingCadenceAndSpeedNotificationService> cscNotificationService;
    std::shared_ptr<PowerNotificationService> powerNotificationService;
    std::shared_ptr<FecService> fecService;
    std::shared_ptr<ScannerService> scannerService;
    std::shared_ptr<DeviceRegistry> registry;
};
