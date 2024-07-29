#include <Constants.h>
#include "StyleSheets.h"
#include "TrainerWindow.h"

#include <iostream>
#include <Labels.h>

#include <QGridLayout>

#include "Service.h"

TrainerWindow::TrainerWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent): AppWindow(
    handler, parent) {
    std::cout << "TrainerWindow::TrainerWindow" << std::endl;
    auto *layout = new QGridLayout(this);

    selectTrainerPanel = new SelectDevicePanel(
        &Services::FEC_BIKE_TRAINER,
        Constants::Icons::BIKE_TRAINER,
        Constants::Icons::BIKE_TRAINER_HOVER,
        handler, this
    );

    const auto nextLabel = new ButtonLabel(Constants::Buttons::NEXT, true, this);
    connect(nextLabel, &ButtonLabel::clicked, this, &TrainerWindow::next);

    std::cout << "  before add witdget" << std::endl;
    layout->addWidget(selectTrainerPanel, 0, 0, Qt::AlignCenter);
    std::cout << "  after add witdget" << std::endl;
    layout->addWidget(nextLabel, 1, 0, Qt::AlignCenter);

    const auto centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    setStyleSheet((StyleSheets::THEME_DARK + StyleSheets::SCALE_MEDIUM).data());
}

void TrainerWindow::deviceSelected(const std::shared_ptr<Device> &device) {
    std::cout << "TrainerWindow::deviceSelected" << std::endl;
    selectTrainerPanel->deviceSelected(device);
}

void TrainerWindow::next() {
    std::cout << "TrainerWindow::next" << std::endl;
    controllerHandler->next(Constants::Screens::SENSORS);
}
