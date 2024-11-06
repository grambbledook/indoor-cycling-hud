#include "ReconnectPacer.h"

#include <qtconcurrentrun.h>
#include <spdlog/spdlog.h>

ReconnectPacer::ReconnectPacer(
    const std::shared_ptr<Reconnector> &reconnector
): reconnector(reconnector) {
    connect(&timer, &QTimer::timeout, this, &ReconnectPacer::tick);
}

auto ReconnectPacer::start() -> void {
    timer.start(500);
    spdlog::info("ReconnectPacer started");
}

auto ReconnectPacer::stop() -> void {
    timer.stop();
    spdlog::info("ReconnectPacer stopped");
}

auto ReconnectPacer::tick() const -> void {
    spdlog::info("ReconnectPacer tick");
    auto reconnect = [this]() {
        reconnector->tok();
    };
    QtConcurrent::run(reconnect);
}
