#include <Constants.h>
#include "SelectDevicePanel.h"
#include "StyleSheets.h"
#include "TrainerWindow.h"

#include <iostream>
#include <Labels.h>

#include <QGridLayout>

TrainerWindow::TrainerWindow(QWidget *parent): AppWindow(parent) {
    auto *layout = new QGridLayout(this);

    const auto trainer_panel = new SelectDevicePanel(
        Constants::Icons::BIKE_TRAINER,
        Constants::Icons::BIKE_TRAINER_HOVER,
        this
    );

    const auto nextLabel = new ButtonLabel(Constants::Buttons::NEXT, true, this);
    connect(nextLabel, &ButtonLabel::clicked, this, &TrainerWindow::next);

    layout->addWidget(trainer_panel, 0, 0, Qt::AlignCenter);
    layout->addWidget(nextLabel, 1, 0, Qt::AlignCenter);

    const auto centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    setStyleSheet((StyleSheets::THEME_DARK + StyleSheets::SCALE_MEDIUM).data());
}

void TrainerWindow::next() {
    std::cout << "TrainerWindow::next" << std::endl;
    emit nextScreen(Constants::Screens::SENSORS);
}
