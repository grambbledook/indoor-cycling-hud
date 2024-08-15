#pragma once
#include "AppWindow.h"
#include "ControllerHandler.h"
#include "WorkoutSummaryPanel.h"

class WorkoutSummaryWindow final : public AppWindow {
    Q_OBJECT

public:
    explicit WorkoutSummaryWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent = nullptr);

    void workoutSummaryReceived(const WorkoutSummary &data) const;

    void next() override;

    void back();
private:
    WorkoutSummaryPanel *summaryPanel;
};
