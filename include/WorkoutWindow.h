#pragma once
#include "AppWindow.h"

class WorkoutWindow final : public AppWindow {
    Q_OBJECT

public:
    explicit WorkoutWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    void next() override;
};
