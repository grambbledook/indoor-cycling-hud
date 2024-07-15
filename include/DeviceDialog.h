#pragma once

#include <QDialog>
#include <QListWidget>
#include <QObject>

class DeviceDialog final : public QDialog {
    Q_OBJECT

public:
    explicit DeviceDialog(QWidget *parent = nullptr);

    void closeEvent(QCloseEvent *event) override;

    struct Device {
        std::string name;
        std::string address;
    };

signals:
    void deviceSelected(std::shared_ptr<Device> device);

public slots:
    void itemSelected(const QListWidgetItem *item);

    void itemConfirmed(const QListWidgetItem *item);

private:
    std::shared_ptr<QListWidget> listWidget;
    std::shared_ptr<Device> selectedItem;
};
