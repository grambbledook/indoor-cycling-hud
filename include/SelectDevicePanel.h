#pragma once
#include <QMainWindow>
#include <QWidget>

class SelectDevicePanel final : public QMainWindow {
    Q_OBJECT

public:
    SelectDevicePanel(
        const std::string &normal_icon_path,
        const std::string &highlighted_icon_path,
        QWidget *parent = nullptr
    );

    //
    // public slots:
    //     void showSelectDeviceDialog();
    //
    // private:
    //     // DeviceDialogog *dialog;
    //     QString highlighted_icon_path;
    //     QString normal_icon_path;
    //     ClickableLabel *selectIcon;
    //     ValueLabel *metricLabel;
};
