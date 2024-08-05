#pragma once

#include <QLabel>
#include <string>
#include <QWidget>

class LabelSize {
public:
    static const LabelSize SMALL;
    static const LabelSize MEDIUM;
    static const LabelSize LARGE;

    [[nodiscard]] std::string asString() const { return value; }

private:
    explicit LabelSize(std::string value) : value(std::move(value)) {
    }

    std::string value;
};

class TextLabel : public QLabel {
    Q_OBJECT

public:
    explicit TextLabel(std::string text, const LabelSize &size, QWidget *parent = nullptr);

private:
    std::string text;
};

class ValueLabel final : public TextLabel {
    Q_OBJECT

public:
    explicit ValueLabel(std::string text, const LabelSize &size, QWidget *parent = nullptr);
};

class ButtonLabel final : public TextLabel {
    Q_OBJECT

public:
    explicit ButtonLabel(std::string text, bool borders = true, QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent *event) override;
signals:
    void clicked();
};

class ClickableLabel final : public QLabel {
    Q_OBJECT

public:
    explicit ClickableLabel(
        const std::string &normal_icon_path,
        const std::string &highlighted_icon_path = "nullptr",
        QWidget *parent = nullptr
    );

    void enterEvent(QEnterEvent *event) override;

    void leaveEvent(QEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

signals:
    void clicked();

private:
    static std::shared_ptr<QPixmap> pixmap(const std::string &path);

    std::shared_ptr<QPixmap> normal;
    std::shared_ptr<QPixmap> highlighted;
};
