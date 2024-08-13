#include "DeviceDialog.h"

#include <spdlog/spdlog.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <QListWidget>
#include <QPainter>

#include "Constants.h"
#include "Labels.h"
#include "StyleSheets.h"

DeviceDialog::DeviceDialog(
    const std::vector<std::shared_ptr<Device>> &data,
    const std::shared_ptr<ControllerHandler> &handler, QWidget *parent
) : QDialog(parent),
    handler(handler), listWidget(std::make_shared<QListWidget>(this)) {
    connect(listWidget.get(), &QListWidget::itemClicked, this, &DeviceDialog::itemSelected);
    connect(listWidget.get(), &QListWidget::itemDoubleClicked, this, &DeviceDialog::itemConfirmed);

    for (const auto &device : data) {
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

    const auto centralWidget = new QWidget(this);
    centralWidget->setObjectName(Constants::Classes::PANEL);
    centralWidget->setLayout(layout);

    const auto mainLayout = new QVBoxLayout();
    mainLayout->addWidget(centralWidget);
    setLayout(mainLayout);

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

    const auto existingItems = listWidget->findItems(deviceName, Qt::MatchFlag::MatchExactly);

    if (existingItems.empty()) {
        listWidget->addItem(item);
        return;
    }

    const auto itemToReplace = existingItems.first();
    const auto row = listWidget->row(itemToReplace);
    listWidget->insertItem(row, item);
    listWidget->takeItem(row + 1);
}

void DeviceDialog::itemSelected(const QListWidgetItem *item) {
    spdlog::info("DeviceDialog::itemSelected");

    auto data = item->data(Qt::ItemDataRole::UserRole);
    if (data.isNull()) {
        spdlog::info("  No data found");
    }

    const auto device = get<std::shared_ptr<Device>>(data);

    spdlog::info("  Selected device: {}", device->name.value);
    selectedItem = device;
}

void DeviceDialog::closeEvent(QCloseEvent *event) {
    spdlog::info("DeviceDialog::closeEvent");

    if (selectedItem) {
        spdlog::info("  DeviceDialog::closeEvent: emitting deviceSelected {}", fmt::ptr(selectedItem));
    }

    handler->next(Constants::Commands::CONNECT);
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

    const std::shared_ptr<Device> device = deviceEvent->getEvent().device;
    renderDevice(device);
    return true;
}