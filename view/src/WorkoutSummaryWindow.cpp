#include "WorkoutSummaryWindow.h"

#include <Constants.h>
#include "TrainerWindow.h"

#include <spdlog/spdlog.h>
#include <Labels.h>

#include <QGridLayout>
#include <QListWidget>

#include "Events.h"

WorkoutSummaryWindow::WorkoutSummaryWindow(const std::shared_ptr<ControllerHandler> &handler,
                                           QWidget *parent): AppWindow(
    handler, parent) {
    const auto listWidget = new QListWidget(this);
    listWidget->addItem("Heart rate: ...");
    listWidget->addItem("Speed: ...");
    listWidget->addItem("Cadence: ...");
    listWidget->addItem("Power: ...");
    listWidget->addItem("Duration: ...");

    const auto startLabel = new ButtonLabel(Constants::Buttons::START, true, this);
    connect(startLabel, &ButtonLabel::clicked, this, &WorkoutSummaryWindow::back);

    const auto exitLabel = new ButtonLabel(Constants::Buttons::EXIT, true, this);
    connect(exitLabel, &ButtonLabel::clicked, this, &WorkoutSummaryWindow::next);

    auto *layout = new QGridLayout(this);
    layout->addWidget(listWidget, 0, 0, 1, 2, Qt::AlignCenter);
    layout->addWidget(startLabel, 1, 0, Qt::AlignRight);
    layout->addWidget(exitLabel, 1, 1, Qt::AlignRight);
    layout->setColumnStretch(1, 1);

    const auto centralWidget = new QWidget(this);
    centralWidget->setObjectName(Constants::Classes::PANEL);
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);
}

void WorkoutSummaryWindow::next() {
    controllerHandler->next(Constants::Commands::QUIT);
}

void WorkoutSummaryWindow::back() {
    controllerHandler->next(Constants::Screens::SELECT_WORKOUT);
}
