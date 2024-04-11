from PySide6.QtCore import Qt
from PySide6.QtWidgets import QGridLayout, QWidget, QVBoxLayout, QHBoxLayout, QSpacerItem, QSizePolicy, QFrame

from hud.configuration.config import Config
from hud.model.model import Model
from hud.view import DeviceController
from hud.view.primitives.labels import ButtonLabel, ValueLabel, LabelSize, TextLabel
from hud.view.primitives.theme_switch import with_switchable_theme


@with_switchable_theme
class MetricsPanel(QWidget):
    def __init__(
            self,
            text: str,
            controller: DeviceController,
            model: Model,
            app_config: Config,
            parent=None
    ):
        super().__init__(parent)
        self.app_config = app_config
        self.dialog_refresher = None

        self.model = model
        self.controller = controller

        panel_layout = QVBoxLayout()
        button_layout = QHBoxLayout()
        value_layout = QGridLayout()

        self.left_button = ButtonLabel('<', self.app_config.hud_layout.theme, with_borders=False)
        self.right_button = ButtonLabel('>', self.app_config.hud_layout.theme, with_borders=False)

        self.label = TextLabel(text, self.app_config.hud_layout.theme, size=LabelSize.SMALL)
        self.value_label = ValueLabel("0", self.app_config.hud_layout.theme, size=LabelSize.LARGE)

        value_layout.addWidget(self.label, 0, 0, 1, 1, Qt.AlignmentFlag.AlignCenter)
        value_layout.addItem(
            QSpacerItem(20, 10, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding), 1, 0, 1, 1,
            Qt.AlignmentFlag.AlignCenter
        )
        value_layout.addWidget(self.value_label, 2, 0, 1, 1, Qt.AlignmentFlag.AlignCenter)

        button_layout.addWidget(self.left_button, Qt.AlignmentFlag.AlignLeft)
        button_layout.addLayout(value_layout, Qt.AlignmentFlag.AlignCenter)
        button_layout.addWidget(self.right_button, Qt.AlignmentFlag.AlignRight)

        panel_layout.addLayout(button_layout)
        self.panel = QFrame()
        self.panel.setLayout(panel_layout)
        self.panel.setObjectName("MetricsPanel")

        main_layout = QVBoxLayout()
        main_layout.addWidget(self.panel)
        self.setLayout(main_layout)

    def updateDevice(self, value):
        self.panel.setToolTip(str(value))

    def updateMetrics(self, value):
        self.value_label.setText(str(value.latest))

    def closeEvent(self, event):
        event.accept()

    def bind_to_model(self, channel):
        channel.device_selected.subscribe(self.updateDevice)
        channel.metrics.subscribe(self.updateMetrics)
