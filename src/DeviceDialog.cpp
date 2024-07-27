#include "DeviceDialog.h"

#include <iostream>
#include <qboxlayout.h>
#include <qevent.h>
#include <QListWidget>

#include "Constants.h"
#include "Labels.h"
#include "StyleSheets.h"

DeviceDialog::DeviceDialog(QWidget *parent): QDialog(parent), listWidget(std::make_shared<QListWidget>(this)) {
    connect(listWidget.get(), &QListWidget::itemClicked, this, &DeviceDialog::itemSelected);
    connect(listWidget.get(), &QListWidget::itemDoubleClicked, this, &DeviceDialog::itemConfirmed);

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

void DeviceDialog::itemSelected(const QListWidgetItem *item) {
    auto data = item->data(Qt::ItemDataRole::UserRole);

    auto device = get<Device>(data);

    std::cout << "Selected device: " << device.name.value << std::endl;
    selectedItem = std::make_shared<Device>(device);
}

void DeviceDialog::closeEvent(QCloseEvent *event) {
    std::cout << "DeviceDialog::closeEvent" << std::endl;

    if (selectedItem) {
        std::cout << "DeviceDialog::closeEvent: emitting deviceSelected" << selectedItem << std::endl;
        emit deviceSelected(selectedItem);
    }

    event->accept();
}
