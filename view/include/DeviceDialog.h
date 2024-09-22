#pragma once

#include <QDialog>
#include <QListWidget>
#include <QObject>

#include "ControllerHandler.h"
#include "Events.h"

class DeviceDialog final : public QDialog {
    Q_OBJECT

public:
    explicit DeviceDialog(
        const std::vector<std::shared_ptr<Device> > &data,
        const std::shared_ptr<ControllerHandler> &handler,
        QWidget *parent = nullptr
    );

    auto closeEvent(QCloseEvent *event) -> void override;

    auto event(QEvent *event) -> bool override;

public slots:
    auto renderDevice(const std::shared_ptr<Device> &device) const -> void;

    auto itemSelected(const QListWidgetItem *item) -> void;

    auto itemConfirmed(const QListWidgetItem *item) -> void;

public:
    std::shared_ptr<ControllerHandler> handler;
    std::shared_ptr<QListWidget> listWidget;
    std::shared_ptr<Device> selectedItem;
};
