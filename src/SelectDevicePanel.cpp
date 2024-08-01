#include <QMainWindow>
#include <QGridLayout>
#include <QWidget>
#include <QSpacerItem>
#include <QSizePolicy>

#include "SelectDevicePanel.h"

#include <iostream>

#include "StyleSheets.h"

#include <iostream>
#include <qevent.h>

#include "Constants.h"
#include "ControllerHandler.h"
#include "ModelEvents.h"
#include "SupportedServices.h"

SelectDevicePanel::SelectDevicePanel(
    const AppService &service,
    const std::string &normal_icon_path,
    const std::string &highlighted_icon_path,
    const std::shared_ptr<ControllerHandler> &handler,
    QWidget *parent
) : QMainWindow(parent), handler(handler), service(service) {
    selectIcon = new ClickableLabel(normal_icon_path, highlighted_icon_path, this);
    selectIcon->setToolTip("No device selected");
    connect(selectIcon, &ClickableLabel::clicked, this, &SelectDevicePanel::handleDeviceButtonClick);
    metricLabel = new ValueLabel("--/--", LabelSize::MEDIUM, this);
    metricLabel->setToolTip("No device selected");

    const auto spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    const auto layout = new QGridLayout(this);
    layout->addWidget(selectIcon, 0, 0, Qt::AlignCenter);
    layout->addItem(spacer, 1, 0, Qt::AlignCenter);
    layout->addWidget(metricLabel, 2, 0, Qt::AlignCenter);

    const auto centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    setStyleSheet((StyleSheets::THEME_DARK + StyleSheets::SCALE_MEDIUM).data());
}

void SelectDevicePanel::deviceSelected(const DeviceSelected &event) {
    if (event.service != this->service.service) {
        return;
    }

    std::cout << "SelectDevicePanel::deviceSelected" << std::endl;
    const auto name = QString::fromStdString(event.device->name.value);
    if (selectIcon) {
        selectIcon->setToolTip(name);
    }
    if (metricLabel) {
        metricLabel->setToolTip(name);
    }
    setToolTip(name);
}

void SelectDevicePanel::measurementsReceived(const WorkoutData &measurements_update) {
    std::cout << "SelectDevicePanel::measurementsReceived" << std::endl;

    if (service.service == HEART_RATE.service) {
        std::cout << "  SelectDevicePanel::measurementsReceived: HEART_RATE" << std::endl;
        const auto heart_rate = measurements_update.hrm.val;
        metricLabel->setText(QString::number(heart_rate));
    }

    if (service.service == CADENCE.service) {
        std::cout << "  SelectDevicePanel::measurementsReceived: CADENCE" << std::endl;
        const auto cadence = measurements_update.cadence.val;
        metricLabel->setText(QString::number(cadence));
    }

    if (service.service == SPEED.service) {
        std::cout << "  SelectDevicePanel::measurementsReceived: SPEED" << std::endl;
        const auto speed = measurements_update.speed.val;
        metricLabel->setText(QString::number(speed));
    }

    if (service.service == POWER.service) {
        std::cout << "  SelectDevicePanel::measurementsReceived: POWER" << std::endl;
        const auto power = measurements_update.power.val;
        metricLabel->setText(QString::number(power));
    }
}

void SelectDevicePanel::handleDeviceButtonClick() const {
    std::cout << "SelectDevicePanel::show_device_dialog()" << std::endl;
    handler->next(Constants::Screens::DEVICE_DIALOG);
}
