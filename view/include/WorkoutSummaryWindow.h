#pragma once
#include "AppWindow.h"
#include "ControllerHandler.h"
#include "WorkoutSummaryPanel.h"

class WorkoutSummaryWindow final : public AppWindow {
    Q_OBJECT

public:
    explicit WorkoutSummaryWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    auto workoutSummaryReceived(const WorkoutEvent &data) const -> void;

    auto next() -> void override;

    auto back() -> void;
private:
    WorkoutSummaryPanel *summaryPanel;
};
