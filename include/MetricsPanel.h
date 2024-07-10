#pragma once
#include <QWidget>

class MetricsPanel final : public QWidget {
    Q_OBJECT

public:
    explicit MetricsPanel(std::string text, QWidget *parent = nullptr);

private:
    std::string text;
};
