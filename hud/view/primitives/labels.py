import enum

from PySide6.QtCore import Signal
from PySide6.QtGui import QPixmap, QColor, Qt
from PySide6.QtWidgets import QLabel

from hud.configuration.config import Theme


class LabelType(enum.EnumType):
    BUTTON = 28
    CLICKABLE_LABEL = 64
    LABEL = 64


class LabelSize(enum.EnumType):
    SMALL = 1
    MEDIUM = 2
    LARGE = 3


def compute_pixmap(icon_path, label_type, theme) -> QPixmap:
    pixmap = QPixmap(icon_path)
    image = pixmap.toImage()

    for x in range(image.width()):
        for y in range(image.height()):
            color = QColor.fromRgba(image.pixel(x, y))

            if color.alpha() == 0:
                continue
            r, g, b = theme.background_colour
            color.setRgb(r ^ 0xFF, g ^ 0xFF, b ^ 0xFF)
            image.setPixel(x, y, color.rgba())

    pixmap = QPixmap.fromImage(image)
    return pixmap.scaledToHeight(label_type, mode=Qt.TransformationMode.SmoothTransformation)


class ClickableLabel(QLabel):
    clicked = Signal()

    def __init__(self, normal_icon_path: str, highlighted_icon_path: str, theme: Theme,
                 label_size: LabelType = LabelType.BUTTON,
                 parent=None):
        super().__init__(parent)
        self.norm = None
        self.high = None
        self.theme = theme

        self.label_type = label_size
        self.normal_icon_path = normal_icon_path
        self.highlighted_icon_path = highlighted_icon_path

        self.applyTheme(theme)

    def applyTheme(self, theme: Theme):
        self.norm = compute_pixmap(self.normal_icon_path, self.label_type, theme)
        self.high = compute_pixmap(self.highlighted_icon_path, self.label_type, theme)

        self.setPixmap(self.norm)
        self.setStyleSheet(theme.stylesheet)

    def enterEvent(self, event):
        self.setPixmap(self.high)

    def leaveEvent(self, event):
        self.setPixmap(self.norm)

    def mousePressEvent(self, event):
        self.clicked.emit()


class TextLabel(QLabel):
    def __init__(self, text: str, theme: Theme, size: LabelSize = LabelSize.MEDIUM, parent=None):
        super().__init__(parent)
        self.text = text
        self.theme = theme

        match size:
            case LabelSize.SMALL:
                self.setObjectName("small")
            case LabelSize.MEDIUM:
                self.setObjectName("medium")
            case LabelSize.LARGE:
                self.setObjectName("large")

        self.setText(text)
        self.applyTheme(theme)

    def applyTheme(self, theme: Theme):
        self.setStyleSheet(theme.stylesheet)


class ValueLabel(TextLabel):
    def __init__(self, text: str, theme: Theme, size: LabelSize = LabelSize.MEDIUM, parent=None):
        super().__init__(text, theme, size, parent)


class ButtonLabel(TextLabel):
    clicked = Signal()

    def __init__(self, text: str, theme: Theme, with_borders: bool = True, parent=None):
        super().__init__(text, theme, parent)
        if with_borders:
            self.setObjectName("with_borders")

    def mousePressEvent(self, event):
        self.clicked.emit()
