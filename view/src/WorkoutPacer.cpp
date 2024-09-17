#include "WorkoutPacer.h"

#include <spdlog/spdlog.h>

WorkoutPacer::WorkoutPacer(const std::shared_ptr<Model> &model): model(model) {
    connect(&timer, &QTimer::timeout, this, &WorkoutPacer::tick);
}

void WorkoutPacer::start() {
    timer.start(500);
}

void WorkoutPacer::stop() {
    timer.stop();
}

void WorkoutPacer::tick() const {
    spdlog::info("Doing work...");
    model->tick();

}
