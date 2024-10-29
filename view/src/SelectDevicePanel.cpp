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
) : QMainWindow(parent), service(service), handler(handler) {
    selectIcon = new ClickableLabel(normal_icon_path, highlighted_icon_path, this);
    selectIcon->setToolTip("No device selected");
    connect(selectIcon, &ClickableLabel::clicked, this, &SelectDevicePanel::handleDeviceButtonClick);

    sensorNameLabel = new ValueLabel("--/--", LabelSize::MEDIUM, this);
    sensorNameLabel->setToolTip("No device selected");

    const auto spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    const auto layout = new QGridLayout(this);
    layout->addWidget(selectIcon, 0, 0, Qt::AlignCenter);
    layout->addItem(spacer, 1, 0, Qt::AlignCenter);
    layout->addWidget(sensorNameLabel, 2, 0, Qt::AlignCenter);

    const auto centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    setStyleSheet((StyleSheets::THEME_DARK + StyleSheets::SCALE_MEDIUM).data());
}

auto SelectDevicePanel::deviceSelected(const DeviceSelected &event) -> void {
    if (event.service != this->service.service) {
        return;
    }

    const auto name = QString::fromStdString(event.device->name.value.substr(0, 10));
    if (selectIcon) {
        selectIcon->setToolTip(name);
    }
    if (sensorNameLabel) {
        sensorNameLabel->setToolTip(name);
    }
    setToolTip(name);
    sensorNameLabel->setText(name);
}

auto SelectDevicePanel::handleDeviceButtonClick() const -> void {
    handler->next(Constants::Screens::DEVICE_DIALOG_SHOW);
}
