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

    void closeEvent(QCloseEvent *event) override;

    bool event(QEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

public slots:
    void renderDevice(const std::shared_ptr<Device> &device) const;

    void itemSelected(const QListWidgetItem *item);

    void itemConfirmed(const QListWidgetItem *item);

public:
    std::shared_ptr<ControllerHandler> handler;
    std::shared_ptr<QListWidget> listWidget;
    std::shared_ptr<Device> selectedItem;
};
