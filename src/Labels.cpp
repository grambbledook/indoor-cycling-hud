#include "Labels.h"

#include <memory>

#include "Constants.h"
#include "StyleSheets.h"

const LabelSize LabelSize::SMALL = LabelSize("small");
const LabelSize LabelSize::MEDIUM = LabelSize("medium");
const LabelSize LabelSize::LARGE = LabelSize("large");

ClickableLabel::ClickableLabel(
    const std::string &normal_icon_path,
    const std::string &highlighted_icon_path,
    QWidget *parent
) : QLabel(parent), normal(pixmap(normal_icon_path)), highlighted(pixmap(highlighted_icon_path)) {
    setPixmap(*normal);
    setStyleSheet((StyleSheets::THEME_DARK + StyleSheets::SCALE_MEDIUM).data());
}

void ClickableLabel::enterEvent(QEnterEvent *event) {
    setPixmap(*highlighted);
}

void ClickableLabel::leaveEvent(QEvent *event) {
    setPixmap(*normal);
}

void ClickableLabel::mousePressEvent(QMouseEvent *event) {
    emit clicked();
}

std::shared_ptr<QPixmap> ClickableLabel::pixmap(const std::string &path) {
    const auto pixmap = std::make_unique<QPixmap>(QString::fromStdString(path));
    auto image = pixmap->toImage();

    for (auto x = 0; x < image.width(); x++) {
        for (auto y = 0; y < image.height(); y++) {
            auto color = QColor::fromRgba(image.pixel(x, y));
            if (color.alpha() == 0) {
                continue;
            }
            auto r = 0xFF ^ 0xFF;
            auto g = 0xFF ^ 0xFF;
            auto b = 0xFF ^ 0xFF;
            color.setRgb(r, g, b);
            image.setPixel(x, y, color.rgba());
        }
    }
    return std::make_shared<QPixmap>(QPixmap::fromImage(image));
}


TextLabel::TextLabel(std::string text, const LabelSize &size, QWidget *parent) : QLabel(parent), text(std::move(text)) {
    setText(QString::fromStdString(this->text));
    setObjectName(size.asString());
    setStyleSheet((StyleSheets::THEME_DARK + StyleSheets::SCALE_MEDIUM).data());
}

ValueLabel::ValueLabel(std::string text, const LabelSize &size, QWidget *parent) : TextLabel(
    std::move(text), size, parent) {
}

ButtonLabel::ButtonLabel(std::string text, const bool borders, QWidget *parent): TextLabel(
    std::move(text), LabelSize::SMALL, parent
) {
    if (borders) {
        setObjectName(Constants::Classes::WITH_BORDERS);
    }
}

void ButtonLabel::mousePressEvent(QMouseEvent *event) {
    emit clicked();
}
