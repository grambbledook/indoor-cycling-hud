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
#include "SelectWorkoutWindow.h"
#include "DeviceWindow.h"
#include "WorkoutPacer.h"
#include "WorkoutSummaryWindow.h"
#include "WorkoutWindow.h"

template<typename V>
class Controller {
public:
    virtual ~Controller() = default;

    virtual auto handleRequest(V v)-> void = 0;
};

template<typename T, typename = std::enable_if_t<std::is_base_of_v<QWidget, T> > >
class WidgetController : public Controller<void *> {
public:
    ~WidgetController() override = default;

    explicit WidgetController(
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history)
        : history(history), state(state) {
    }

    auto handleRequest(void * = nullptr) -> void override = 0;

protected:
    std::shared_ptr<AppState> state;
    std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > history;
};

template<typename T, typename = std::enable_if_t<std::is_base_of_v<QWidget, T> > >
class ViewController : public WidgetController<T> {
public:
    explicit ViewController(
        const std::shared_ptr<T> &view,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history)
        : WidgetController<T>(state, history), view(view) {
    }

    auto renderView() -> void;

    std::shared_ptr<T> view;
};

class UeberWindowController final : public ViewController<DeviceWindow> {
public:
    explicit UeberWindowController(
        const std::shared_ptr<DeviceWindow> &view,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history)
        : ViewController(view, state, history) {
    }

    auto handleRequest(void * = nullptr) -> void override;
};

class SelectWorkoutWindowController final : public ViewController<SelectWorkoutWindow> {
public:
    explicit SelectWorkoutWindowController(
        const std::shared_ptr<SelectWorkoutWindow> &view,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history
    )
        : ViewController(view, state, history) {
    }

    auto handleRequest(void * = nullptr) -> void override;
};

class WorkoutWindowController final : public ViewController<WorkoutWindow> {
public:
    explicit WorkoutWindowController(
        const std::shared_ptr<WorkoutWindow> &view,
        const std::shared_ptr<Model> &model,
        const std::shared_ptr<WorkoutPacer> &pacer,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history
    )
        : ViewController(view, state, history), model(model), pacer(pacer) {
    }

    auto handleRequest(void * = nullptr) -> void override;

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<WorkoutPacer> pacer;
};

class WorkoutSummaryWindowController final : public ViewController<WorkoutSummaryWindow> {
public:
    explicit WorkoutSummaryWindowController(
        const std::shared_ptr<WorkoutSummaryWindow> &view,
        const std::shared_ptr<Model> &model,
        const std::shared_ptr<WorkoutPacer> &pacer,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history
    )
        : ViewController(view, state, history), model(model), pacer(pacer) {
    }

    auto handleRequest(void * = nullptr) -> void override;

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<WorkoutPacer> pacer;
};

class ShowDeviceDialogController final : public WidgetController<DeviceDialog> {
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
        : WidgetController(state, history), createDialog(createDialog), model(model), scannerService(scannerService),
          qtAdapter(qtAdapter) {
    }

    auto handleRequest(void * = nullptr) -> void override;

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<ScannerService> scannerService;
    std::function<std::shared_ptr<DeviceDialog>(std::vector<std::shared_ptr<Device> >, QWidget *)> createDialog;
    std::shared_ptr<QtEventPublisher> qtAdapter;
};

class ConnectToDeviceController final : public WidgetController<DeviceDialog> {
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
        : WidgetController(state, history), hrmNotificationService(hrmNotificationService),
          cscNotificationService(cscNotificationService),
          powerNotificationService(powerNotificationService),
          fecService(fecService), scannerService(scannerService) {
    }

    auto handleRequest(void * = nullptr) -> void override;

private:
    std::shared_ptr<HrmNotificationService> hrmNotificationService;
    std::shared_ptr<CyclingCadenceAndSpeedNotificationService> cscNotificationService;
    std::shared_ptr<PowerNotificationService> powerNotificationService;
    std::shared_ptr<FecService> fecService;
    std::shared_ptr<ScannerService> scannerService;
};

class SwitchThemeController final : public WidgetController<QWidget> {
public:
    explicit SwitchThemeController(
        QApplication *app,
        const std::shared_ptr<DeviceWindow> &deviceWindow,
        const std::shared_ptr<SelectWorkoutWindow> &selectWorkoutWindow,
        const std::shared_ptr<WorkoutWindow> &workoutWindow,
        const std::shared_ptr<WorkoutSummaryWindow> &workoutSummaryWindow,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history
    )
        : WidgetController(state, history), app(app), deviceWindow(deviceWindow),
          workoutWindow(workoutWindow), selectWorkoutWindow(selectWorkoutWindow),
          workoutSummaryWindow(workoutSummaryWindow) {
    }

    auto handleRequest(void * = nullptr) -> void override;

private:
    QApplication *app;
    std::shared_ptr<DeviceWindow> deviceWindow;
    std::shared_ptr<WorkoutWindow> workoutWindow;
    std::shared_ptr<SelectWorkoutWindow> selectWorkoutWindow;
    std::shared_ptr<WorkoutSummaryWindow> workoutSummaryWindow;
};

class ShutdownController final : public Controller<void *> {
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
        : hrmNotificationService(hrmNotificationService),
          cscNotificationService(cscNotificationService), powerNotificationService(powerNotificationService),
          fecService(fecService), scannerService(scannerService), state(state), registry(registry) {
    }

    auto handleRequest(void * = nullptr) -> void override;

private:
    std::shared_ptr<HrmNotificationService> hrmNotificationService;
    std::shared_ptr<CyclingCadenceAndSpeedNotificationService> cscNotificationService;
    std::shared_ptr<PowerNotificationService> powerNotificationService;
    std::shared_ptr<FecService> fecService;
    std::shared_ptr<ScannerService> scannerService;
    std::shared_ptr<AppState> state;
    std::shared_ptr<DeviceRegistry> registry;
};

class WheelSizeSelectionController final : public Controller<WheelSize> {
public:
    explicit WheelSizeSelectionController(const std::shared_ptr<Model> &model)
        : model(model) {
    }

    auto handleRequest(WheelSize size)  -> void override;

private:
    std::shared_ptr<Model> model;
};

class SpeedUnitController final : public Controller<DistanceUnit> {
public:
    explicit SpeedUnitController(const std::shared_ptr<Model> &model)
        : model(model) {
    }

    auto handleRequest(DistanceUnit size) -> void override;

private:
    std::shared_ptr<Model> model;
};


class TrayConnectToDeviceController final : public Controller<const std::shared_ptr<Device> &> {
public:
    explicit TrayConnectToDeviceController(
        const std::shared_ptr<HrmNotificationService> &hrmNotificationService,
        const std::shared_ptr<CyclingCadenceAndSpeedNotificationService> &cscNotificationService,
        const std::shared_ptr<PowerNotificationService> &powerNotificationService,
        const std::shared_ptr<FecService> &fecService,
        const std::shared_ptr<AppState> &state
    )
        : hrmNotificationService(hrmNotificationService),
          cscNotificationService(cscNotificationService),
          powerNotificationService(powerNotificationService),
          fecService(fecService), state(state) {
    }

    auto handleRequest(const std::shared_ptr<Device> &device) -> void override;

private:
    std::shared_ptr<HrmNotificationService> hrmNotificationService;
    std::shared_ptr<CyclingCadenceAndSpeedNotificationService> cscNotificationService;
    std::shared_ptr<PowerNotificationService> powerNotificationService;
    std::shared_ptr<FecService> fecService;
    std::shared_ptr<AppState> state;
};
