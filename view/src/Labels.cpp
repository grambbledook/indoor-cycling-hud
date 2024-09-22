#include "Labels.h"

#include <memory>
#include <qcoreapplication.h>
#include <qfile.h>
#include <QDir>
#include <spdlog/spdlog.h>

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

auto ClickableLabel::enterEvent(QEnterEvent *event) -> void {
    setPixmap(*highlighted);
}

auto ClickableLabel::leaveEvent(QEvent *event) -> void {
    setPixmap(*normal);
}

auto ClickableLabel::mousePressEvent(QMouseEvent *event) -> void {
    emit clicked();
}

auto ClickableLabel::pixmap(const std::string &path) -> std::shared_ptr<QPixmap> {
    const auto exeDir = QCoreApplication::applicationDirPath();
    const auto absolutePath = QDir(exeDir).absoluteFilePath(QString::fromStdString(path));

    spdlog::info("Loading image from path: {}", absolutePath.toStdString());
    const auto pixmap = std::make_unique<QPixmap>(absolutePath);
    if (!pixmap->isNull()) {
        spdlog::info("  Image of size {}x{} loaded successfully.", pixmap->width(), pixmap->height());
    } else {
        spdlog::error("  Failed to load image.");
    }

    auto image = pixmap->toImage();
    for (auto x = 0; x < image.width(); x++) {
        for (auto y = 0; y < image.height(); y++) {
            auto color = QColor::fromRgba(image.pixel(x, y));
            if (color.alpha() == 0) {
                continue;
            }
            constexpr auto r = 0xFF;
            constexpr auto g = 0xFF;
            constexpr auto b = 0xFF;
            color.setRgb(r, g, b);
            image.setPixel(x, y, color.rgba());
        }
    }
    return std::make_shared<QPixmap>(QPixmap::fromImage(image));
}

TextLabel::TextLabel(
    std::string text,
    const LabelSize &size,
    QWidget *parent
) : QLabel(parent), text(std::move(text)) {
    setText(QString::fromStdString(this->text));
    setObjectName(size.asString());
    setStyleSheet((StyleSheets::THEME_DARK + StyleSheets::SCALE_MEDIUM).data());
}

ValueLabel::ValueLabel(std::string text, const LabelSize &size, QWidget *parent) : TextLabel(
    std::move(text), size, parent) {
}

ButtonLabel::ButtonLabel(
    std::string text,
    const bool borders,
    QWidget *parent
): TextLabel(std::move(text), LabelSize::SMALL, parent) {
    if (!borders) {
        return;
    }

    setObjectName(Constants::Classes::WITH_BORDERS);
}

auto ButtonLabel::mousePressEvent(QMouseEvent *event) -> void {
    emit clicked();
}
