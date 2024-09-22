#include "WorkoutSummaryWindow.h"

#include <Constants.h>
#include "TrainerWindow.h"

#include <spdlog/spdlog.h>
#include <Labels.h>

#include <QGridLayout>
#include <QTableWidget>
#include <QHeaderView>

#include "Events.h"

WorkoutSummaryWindow::WorkoutSummaryWindow(
    const std::shared_ptr<ControllerHandler> &handler,
    QWidget *parent
): AppWindow(handler, parent) {

    eventHandlers.insert({
        getWorkoutSummaryType(), [this](QEvent *event) {
            const auto data = dynamic_cast<WorkoutSummaryEvent *>(event);
            workoutSummaryReceived(data->getData());
        }
    });

    summaryPanel = new WorkoutSummaryPanel(this);
    const auto startLabel = new ButtonLabel(Constants::Buttons::START, true, this);
    connect(startLabel, &ButtonLabel::clicked, this, &WorkoutSummaryWindow::back);

    const auto exitLabel = new ButtonLabel(Constants::Buttons::EXIT, true, this);
    connect(exitLabel, &ButtonLabel::clicked, this, &WorkoutSummaryWindow::next);

    auto *layout = new QGridLayout(this);
    layout->addWidget(summaryPanel, 0, 0, 1, 2, Qt::AlignCenter);
    layout->addWidget(startLabel, 1, 0, Qt::AlignRight);
    layout->addWidget(exitLabel, 1, 1, Qt::AlignRight);
    layout->setColumnStretch(1, 1);

    const auto centralWidget = new QWidget(this);
    centralWidget->setObjectName(Constants::Classes::PANEL);
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);
}

auto WorkoutSummaryWindow::workoutSummaryReceived(const WorkoutEvent &data) const -> void  {
    summaryPanel->handleWorkoutEvent(data);
}

auto WorkoutSummaryWindow::next() -> void  {
    controllerHandler->next(Constants::Commands::QUIT);
}

auto WorkoutSummaryWindow::back() -> void  {
    controllerHandler->next(Constants::Screens::SELECT_WORKOUT);
}
