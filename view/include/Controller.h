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
#include "Reconnector.h"
#include "ReconnectPacer.h"
#include "SettingsManager.h"
#include "WorkoutPacer.h"
#include "WorkoutSummaryWindow.h"
#include "WorkoutWindow.h"

template<typename V>
class Controller {
public:
    virtual ~Controller() = default;
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

class StartupController final : public Controller<void *> {
public:
    explicit StartupController(
        const std::shared_ptr<SettingsManager> &initializer,
        const std::shared_ptr<ReconnectPacer> &pacer,
        const std::shared_ptr<ControllerHandler> &handler,
        const std::shared_ptr<AppState> &state
    )
        : initializer(initializer), pacer(pacer), handler(handler), state(state) {
    }

    auto start() const -> void;

private:
    std::shared_ptr<SettingsManager> initializer;
    std::shared_ptr<ReconnectPacer> pacer;
    std::shared_ptr<ControllerHandler> handler;
    std::shared_ptr<AppState> state;
};


class DeviceWindowController final : public ViewController<DeviceWindow> {
public:
    explicit DeviceWindowController(
        const std::shared_ptr<DeviceWindow> &view,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history)
        : ViewController(view, state, history) {
    }

    auto showDeviceWindow() -> void;
};

class SelectWorkoutWindowController final : public ViewController<SelectWorkoutWindow> {
public:
    explicit SelectWorkoutWindowController(
        const std::shared_ptr<ScannerService> &scannerService,
        const std::shared_ptr<SelectWorkoutWindow> &view,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history
    )
        : ViewController(view, state, history), scannerService(scannerService) {
    }

    auto showSelectWorkoutWindow() -> void;

private:
    std::shared_ptr<ScannerService> scannerService;
};

class WorkoutWindowController final : public ViewController<WorkoutWindow> {
public:
    explicit WorkoutWindowController(
        const std::shared_ptr<ScannerService> &scannerService,
        const std::shared_ptr<WorkoutWindow> &view,
        const std::shared_ptr<Model> &model,
        const std::shared_ptr<WorkoutPacer> &pacer,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history
    )
        : ViewController(view, state, history), model(model), scannerService(scannerService), pacer(pacer) {
    }

    auto showWorkoutWindow() -> void;

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<ScannerService> scannerService;
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

    auto showWorkoutSummaryWindow() -> void;

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<WorkoutPacer> pacer;
};

class DeviceDialogController final : public WidgetController<DeviceDialog> {
public:
    explicit DeviceDialogController(
        const std::shared_ptr<QtEventPublisher> &qtAdapter,
        const std::function<std::shared_ptr<DeviceDialog>(std::vector<std::shared_ptr<Device> >, QWidget *)> &
        createDialog,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history,
        const std::shared_ptr<Model> &model
    ) : WidgetController(state, history),
        model(model),
        createDialog(createDialog),
        qtAdapter(qtAdapter) {
    }

    auto showDialog() const -> void;

    auto closeDialog() const -> void;

private:
    std::shared_ptr<Model> model;

    std::function<std::shared_ptr<DeviceDialog>(std::vector<std::shared_ptr<Device> >, QWidget *)> createDialog;
    std::shared_ptr<QtEventPublisher> qtAdapter;
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

    auto switchColorTheme() const -> void;

private:
    QApplication *app;
    std::shared_ptr<DeviceWindow> deviceWindow;
    std::shared_ptr<WorkoutWindow> workoutWindow;
    std::shared_ptr<SelectWorkoutWindow> selectWorkoutWindow;
    std::shared_ptr<WorkoutSummaryWindow> workoutSummaryWindow;
};

class WheelSizeSelectionController final : public Controller<WheelSize> {
public:
    explicit WheelSizeSelectionController(const std::shared_ptr<Model> &model)
        : model(model) {
    }

    auto setWheelSize(WheelSize size) const -> void;

private:
    std::shared_ptr<Model> model;
};

class SpeedUnitController final : public Controller<DistanceUnit> {
public:
    explicit SpeedUnitController(const std::shared_ptr<Model> &model)
        : model(model) {
    }

    auto setDistanceUnit(DistanceUnit size) const -> void;

private:
    std::shared_ptr<Model> model;
};


class ConnectToDeviceController final : public Controller<const std::shared_ptr<Device> &> {
public:
    explicit ConnectToDeviceController(
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

    auto connectToDevice(const std::shared_ptr<Device> &device) const -> void;

private:
    std::shared_ptr<HrmNotificationService> hrmNotificationService;
    std::shared_ptr<CyclingCadenceAndSpeedNotificationService> cscNotificationService;
    std::shared_ptr<PowerNotificationService> powerNotificationService;
    std::shared_ptr<FecService> fecService;
    std::shared_ptr<AppState> state;
};

class DeviceReconnectionController final : public Controller<void *> {
public:
    explicit DeviceReconnectionController(
        const std::shared_ptr<Reconnector> &reconnector,
        const std::shared_ptr<AppState> &state
    ): reconnector(reconnector), state(state) {
    }

    auto reconnect(const std::shared_ptr<Device> &device) const -> void;

    auto connected(const std::shared_ptr<Device> &device) const -> void;

private:
    const std::shared_ptr<Reconnector> reconnector;
    const std::shared_ptr<AppState> state;
};


class ShutdownController final : public Controller<void *> {
public:
    explicit ShutdownController(
        const std::shared_ptr<SettingsManager> &initializer,
        const std::shared_ptr<HrmNotificationService> &hrmNotificationService,
        const std::shared_ptr<CyclingCadenceAndSpeedNotificationService> &cscNotificationService,
        const std::shared_ptr<PowerNotificationService> &powerNotificationService,
        const std::shared_ptr<FecService> &fecService,
        const std::shared_ptr<ScannerService> &scannerService,
        const std::shared_ptr<DeviceRegistry> &registry,
        const std::shared_ptr<ReconnectPacer> &pacer,
        const std::shared_ptr<AppState> &state
    )
        : initializer(initializer), hrmNotificationService(hrmNotificationService),
          cscNotificationService(cscNotificationService), powerNotificationService(powerNotificationService),
          fecService(fecService), scannerService(scannerService), pacer(pacer), state(state), registry(registry) {
    }

    auto shutdown() const -> void;

private:
    std::shared_ptr<SettingsManager> initializer;
    std::shared_ptr<HrmNotificationService> hrmNotificationService;
    std::shared_ptr<CyclingCadenceAndSpeedNotificationService> cscNotificationService;
    std::shared_ptr<PowerNotificationService> powerNotificationService;
    std::shared_ptr<FecService> fecService;
    std::shared_ptr<ScannerService> scannerService;
    std::shared_ptr<ReconnectPacer> pacer;
    std::shared_ptr<AppState> state;
    std::shared_ptr<DeviceRegistry> registry;
};
