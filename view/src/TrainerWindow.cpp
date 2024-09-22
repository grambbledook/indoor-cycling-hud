#include <Constants.h>
#include "StyleSheets.h"
#include "TrainerWindow.h"

#include <spdlog/spdlog.h>
#include <Labels.h>

#include <QGridLayout>

#include "BluetoothConstants.h"
#include "Events.h"
#include "SupportedServices.h"

TrainerWindow::TrainerWindow(
    const std::shared_ptr<ControllerHandler> &handler,
    QWidget *parent
): AppWindow(handler, parent) {
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

auto TrainerWindow::deviceSelected(const DeviceSelected &event) const -> void {
    spdlog::info("TrainerWindow::deviceSelected");
    selectTrainerPanel->deviceSelected(event);
}

auto TrainerWindow::next() -> void {
    spdlog::info("TrainerWindow::next");
    controllerHandler->next(Constants::Screens::SENSORS);
}
