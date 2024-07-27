#pragma once
#include <QWidget>
#include <type_traits>
#include <utility>
#include <stack>
#include <memory>

#include "AppState.h"
#include "DeviceDialog.h"
#include "Model.h"
#include "SensorsWindow.h"
#include "TrainerWindow.h"
#include "WorkoutWindow.h"


template<typename T, typename = std::enable_if_t<std::is_base_of_v<QWidget, T> > >
class Controller {
public:
    virtual ~Controller() = default;

    explicit Controller(
        const std::shared_ptr<Model> &model,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history)
        : model(model), state(state), history(history) {
    }

    virtual void handleRequest() = 0;

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
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history,
        const std::shared_ptr<Model> &model)
        : Controller<T>(model, state, history), view(view) {
    }

public:
    std::shared_ptr<T> view;
};

class TrainerWindowController final : public ViewController<TrainerWindow> {
public:
    explicit TrainerWindowController(
        const std::shared_ptr<TrainerWindow> &view,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history,
        const std::shared_ptr<Model> &model)
        : ViewController(view, state, history, model) {
    }

    void handleRequest() override;
};

class SensorsWindowController final : public ViewController<SensorsWindow> {
public:
    explicit SensorsWindowController(
        const std::shared_ptr<SensorsWindow> &view,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history,
        const std::shared_ptr<Model> &model)
        : ViewController(view, state, history, model) {
    }

    void handleRequest() override;
};

class WorkoutWindowController final : public ViewController<WorkoutWindow> {
public:
    explicit WorkoutWindowController(
        const std::shared_ptr<WorkoutWindow> &view,
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history,
        const std::shared_ptr<Model> &model)
        : ViewController(view, state, history, model) {
    }

    void handleRequest() override;
};

class DeviceDialogController final : public Controller<DeviceDialog> {
public:
    explicit DeviceDialogController(
        const std::shared_ptr<AppState> &state,
        const std::shared_ptr<std::stack<std::shared_ptr<QWidget> > > &history,
        const std::shared_ptr<Model> &model)
        : Controller(model, state, history) {
    }

    void handleRequest() override;
};
