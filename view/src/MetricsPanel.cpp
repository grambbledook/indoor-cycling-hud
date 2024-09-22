#include "SelectDevicePanel.h"
#include "StyleSheets.h"
#include "WorkoutWindow.h"
#include "MetricsPanel.h"
#include <Labels.h>

#include <QGridLayout>

#include "Constants.h"

MetricsPanel::MetricsPanel(const Data::DataField &dataField, QWidget *parent): QWidget(parent), dataField(dataField) {
    auto const panel_layout = new QVBoxLayout(this);
    auto const buttonLayout = new QHBoxLayout(this);
    auto const valueLayout = new QGridLayout(this);

    auto const leftButton = new ButtonLabel(Constants::Buttons::LEFT, false, this);
    connect(leftButton, &ButtonLabel::clicked, this, &MetricsPanel::prev);

    auto const rightButton = new ButtonLabel(Constants::Buttons::RIGHT, false, this);
    connect(rightButton, &ButtonLabel::clicked, this, &MetricsPanel::next);

    label = new TextLabel(this->dataField.text, LabelSize::SMALL, this);
    valueLabel = new ValueLabel("0", LabelSize::LARGE, this);
    auto const spacer = new QSpacerItem(20, 10, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

    valueLayout->addWidget(label, 0, 0, 1, 1, Qt::AlignmentFlag::AlignCenter);
    valueLayout->addItem(spacer, 1, 0, 1, 1, Qt::AlignmentFlag::AlignCenter);
    valueLayout->addWidget(valueLabel, 2, 0, 1, 1, Qt::AlignmentFlag::AlignCenter);

    buttonLayout->addWidget(leftButton, Qt::AlignmentFlag::AlignLeft);
    buttonLayout->addLayout(valueLayout, Qt::AlignmentFlag::AlignCenter);
    buttonLayout->addWidget(rightButton, Qt::AlignmentFlag::AlignRight);

    panel_layout->addLayout(buttonLayout);
    auto const panel = new QFrame(this);
    panel->setLayout(panel_layout);
    panel->setObjectName(Constants::Classes::FRAME_WITH_BORDERS);

    auto const main_layout = new QVBoxLayout(this);
    main_layout->addWidget(panel);
    setLayout(main_layout);

    setStyleSheet((StyleSheets::THEME_DARK + StyleSheets::SCALE_MEDIUM).data());
}

auto MetricsPanel::next() -> void {
    auto idx = Data::index(dataField);

    idx = (++idx) % Data::DATA_FIELDS.size();
    dataField = Data::DATA_FIELDS[idx];
    updateTextLabel();
    updateValueLabel();
}

auto MetricsPanel::prev() -> void {
    auto idx = Data::index(dataField);

    idx = ((--idx) + Data::DATA_FIELDS.size()) % Data::DATA_FIELDS.size();
    dataField = Data::DATA_FIELDS[idx];
    updateTextLabel();
    updateValueLabel();
}

auto MetricsPanel::measurementsReceived(const WorkoutEvent &data) -> void {
    this->data = std::make_shared<WorkoutEvent>(data);
    updateValueLabel();
}

auto MetricsPanel::updateTextLabel() const -> void {
    label->setText(QString::fromStdString(dataField.text));
}

auto MetricsPanel::updateValueLabel() const -> void {
    if (!data) {
        valueLabel->setText(QString::fromStdString("--"));
        return;
    }

    valueLabel->setText(QString::fromStdString(dataField.value(*data)));
}
