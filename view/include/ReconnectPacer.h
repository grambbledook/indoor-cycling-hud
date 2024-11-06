#ifndef RECONNECTPACER_H
#define RECONNECTPACER_H
#include <memory>
#include <qtimer.h>

#include "Model.h"
#include "Reconnector.h"


class ReconnectPacer final : public QObject {
    Q_OBJECT

public:
    explicit ReconnectPacer(
        const std::shared_ptr<Reconnector> &reconnector
    );

    auto start() -> void;

    auto stop() -> void;

    auto tick() const -> void;

private:
    std::shared_ptr<Reconnector> reconnector;
    QTimer timer;
};


#endif //RECONNECTPACER_H
