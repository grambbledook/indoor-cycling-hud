#pragma once
#include <QWidget>
#include <type_traits>
#include <utility>
#include <stack>
#include <memory>

#include "AppState.h"
#include "DeviceDialog.h"
#include "SensorsWindow.h"
#include "TrainerWindow.h"
#include "WorkoutWindow.h"


template<typename T, typename = std::enable_if_t<std::is_base_of_v<QWidget, T> > >
class Controller {
public:
    virtual ~Controller() = default;

    explicit Controller(std::shared_ptr<AppState> state,
                        std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history)
        : state(state), history(history) {
    }

    virtual void handleRequest() = 0;

protected:
    std::shared_ptr<AppState> state;
    std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > history;
};

template<typename T, typename = std::enable_if_t<std::is_base_of_v<QWidget, T> > >
class ViewController : public Controller<T> {
public:
    explicit ViewController(
        std::shared_ptr<T> view,
        std::shared_ptr<AppState> state,
        std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history)
        : Controller<T>(state, history), view(view) {
    }

public:
    std::shared_ptr<T> view;
};

class TrainerWindowController final : public ViewController<TrainerWindow> {
public:
    explicit TrainerWindowController(
        std::shared_ptr<TrainerWindow> view,
        std::shared_ptr<AppState> state,
        std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history)
        : ViewController(view, state, history) {
    }

    void handleRequest() override;
};

class SensorsWindowController final : public ViewController<SensorsWindow> {
public:
    explicit SensorsWindowController(
        std::shared_ptr<SensorsWindow> view,
        std::shared_ptr<AppState> state,
        std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history)
        : ViewController(view, state, history) {
    }

    void handleRequest() override;
};

class WorkoutWindowController final : public ViewController<WorkoutWindow> {
public:
    explicit WorkoutWindowController(
        std::shared_ptr<WorkoutWindow> view,
        std::shared_ptr<AppState> state,
        std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history)
        : ViewController(view, state, history) {
    }

    void handleRequest() override;
};

class DeviceDialogController final : public Controller<DeviceDialog> {
public:
    explicit DeviceDialogController(
        std::shared_ptr<AppState> state,
        std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history)
        : Controller(state, history) {
    }

    void handleRequest() override;
};
