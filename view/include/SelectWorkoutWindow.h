#pragma once
#include "AppWindow.h"
#include "ControllerHandler.h"

class SelectWorkoutWindow final : public AppWindow {
    Q_OBJECT

public:
    explicit SelectWorkoutWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    void next() override;
};
