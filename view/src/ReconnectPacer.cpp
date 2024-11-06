#include "ReconnectPacer.h"

#include <qtconcurrentrun.h>
#include <spdlog/spdlog.h>

ReconnectPacer::ReconnectPacer(
    const std::shared_ptr<Reconnector> &reconnector
): reconnector(reconnector) {
    connect(&timer, &QTimer::timeout, this, &ReconnectPacer::tick);
}

auto ReconnectPacer::start() -> void {
    timer.start(1500);
}

auto ReconnectPacer::stop() -> void {
    timer.stop();
}

auto ReconnectPacer::tick() const -> void {
    auto reconnect = [this]() {
        reconnector->tok();
    };
    QtConcurrent::run(reconnect);
}
