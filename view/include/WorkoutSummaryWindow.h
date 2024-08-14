#pragma once
#include "AppWindow.h"
#include "ControllerHandler.h"

class WorkoutSummaryWindow final : public AppWindow {
    Q_OBJECT

public:
    explicit WorkoutSummaryWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    void next() override;

    void back();
};
