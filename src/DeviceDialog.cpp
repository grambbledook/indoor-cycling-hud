#include "DeviceDialog.h"

#include <iostream>
#include <qboxlayout.h>
#include <qevent.h>
#include <QListWidget>

#include "Constants.h"
#include "Events.h"
#include "Labels.h"
#include "StyleSheets.h"

DeviceDialog::DeviceDialog(
    const std::vector<std::shared_ptr<Device> > &data,
    const std::shared_ptr<ControllerHandler> &handler, QWidget *parent
): QDialog(parent),
   handler(handler), listWidget(std::make_shared<QListWidget>(this)) {
    connect(listWidget.get(), &QListWidget::itemClicked, this, &DeviceDialog::itemSelected);
    connect(listWidget.get(), &QListWidget::itemDoubleClicked, this, &DeviceDialog::itemConfirmed);

    for (const auto &device: data) {
        renderDevice(device);
    }
    const auto closeLabel = new ButtonLabel(Constants::Buttons::OK, true, this);
    connect(closeLabel, &ButtonLabel::clicked, this, [this] { this->close(); });

    const auto hLayout = new QHBoxLayout();
    hLayout->addStretch();
    hLayout->addWidget(closeLabel);
    hLayout->addStretch();

    const auto layout = new QVBoxLayout(this);
    layout->addWidget(listWidget.get());
    layout->addLayout(hLayout);

    setLayout(layout);
    setStyleSheet((StyleSheets::THEME_DARK + StyleSheets::SCALE_MEDIUM).data());
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::Dialog);
    setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground);
}

void DeviceDialog::itemConfirmed(const QListWidgetItem *item) {
    itemSelected(item);
    close();
}

void DeviceDialog::renderDevice(const std::shared_ptr<Device> &device) const {
    const auto deviceName = QString::fromStdString(device->name.value);

    const auto item = new QListWidgetItem(deviceName);
    item->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue(device));

    if (const auto alreadyExists = listWidget->findItems(deviceName, Qt::MatchFlag::MatchExactly); not alreadyExists.
        empty()) {
        return;
    }
    listWidget->addItem(item);
}

void DeviceDialog::itemSelected(const QListWidgetItem *item) {
    std::cout << "DeviceDialog::itemSelected" << std::endl;

    auto data = item->data(Qt::ItemDataRole::UserRole);
    if (data.isNull()) {
        std::cout << "  No data found" << std::endl;
    }

    const auto device = get<std::shared_ptr<Device> >(data);

    std::cout << "  Selected device: " << device->name.value << std::endl;
    selectedItem = device;
}

void DeviceDialog::closeEvent(QCloseEvent *event) {
    std::cout << "DeviceDialog::closeEvent" << std::endl;

    if (selectedItem) {
        std::cout << " DeviceDialog::closeEvent: emitting deviceSelected" << selectedItem << std::endl;
    }

    handler->next(Constants::Screens::DEVICE_DIALOG);
    update();
    event->accept();
}

bool DeviceDialog::event(QEvent *event) {
    if (event->type() > QEvent::MaxUser or event->type() < QEvent::User) {
        return QDialog::event(event);
    }

    const auto deviceEvent = dynamic_cast<DeviceDiscoveredEvent *>(event);
    if (not deviceEvent) {
        return QDialog::event(event);
    }

    const std::shared_ptr<Device> device = deviceEvent->getDevice();
    renderDevice(device);
    return true;
}
