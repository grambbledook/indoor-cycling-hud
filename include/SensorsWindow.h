#pragma once
#include "AppWindow.h"

class SensorsWindow final : public AppWindow {
    Q_OBJECT

public:
    explicit SensorsWindow(QWidget *parent = nullptr);

    void back() override;
    void next() override;
};
