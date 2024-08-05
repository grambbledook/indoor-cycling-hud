#include <Constants.h>
#include "StyleSheets.h"
#include "TrainerWindow.h"

#include <iostream>
#include <Labels.h>

#include <QGridLayout>

#include "BluetoothConstants.h"
#include "Events.h"
#include "SupportedServices.h"

TrainerWindow::TrainerWindow(const std::shared_ptr<ControllerHandler> &handler, QWidget *parent): AppWindow(
    handler, parent) {
    std::cout << "TrainerWindow::TrainerWindow" << std::endl;
    const int device_selected_type = getDeviceSelectedType();
    std::cout << "  TrainerWindow::DeviceSelectedType: " << device_selected_type << std::endl;
    eventHandlers.insert({
         getDeviceSelectedType(), [this](QEvent *event) {
            const auto device = dynamic_cast<DeviceSelectedEvent *>(event);
            deviceSelected(device->getEvent());
        }
    });

    auto *layout = new QGridLayout(this);

    selectTrainerPanel = new SelectDevicePanel(
        BIKE_TRAINER,
        Constants::Icons::BIKE_TRAINER,
        Constants::Icons::BIKE_TRAINER_HOVER,
        handler, this
    );

    const auto nextLabel = new ButtonLabel(Constants::Buttons::NEXT, true, this);
    connect(nextLabel, &ButtonLabel::clicked, this, &TrainerWindow::next);

    layout->addWidget(selectTrainerPanel, 0, 0, Qt::AlignCenter);
    layout->addWidget(nextLabel, 1, 0, Qt::AlignCenter);

    const auto centralWidget = new QWidget(this);
    centralWidget->setObjectName(Constants::Classes::PANEL);
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);
}

void TrainerWindow::deviceSelected(const DeviceSelected event) const {
    std::cout << "TrainerWindow::deviceSelected" << std::endl;
    selectTrainerPanel->deviceSelected(event);
}

void TrainerWindow::next() {
    std::cout << "TrainerWindow::next" << std::endl;
    controllerHandler->next(Constants::Screens::SENSORS);
}
