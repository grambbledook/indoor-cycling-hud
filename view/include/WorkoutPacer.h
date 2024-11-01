#ifndef WORKOUTPACER_H
#define WORKOUTPACER_H
#include <memory>
#include <qtimer.h>

#include "Model.h"
#include "Reconnector.h"


class WorkoutPacer : public QObject {
    Q_OBJECT

public:
    explicit WorkoutPacer(
        const std::shared_ptr<Model> &model,
        const std::shared_ptr<Reconnector> &reconnector
    );

    auto start() -> void;

    auto stop() -> void;

    auto tick() const -> void;

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<Reconnector> reconnector;
    QTimer timer;
};


#endif //WORKOUTPACER_H
