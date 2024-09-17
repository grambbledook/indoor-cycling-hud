#ifndef WORKOUTPACER_H
#define WORKOUTPACER_H
#include <memory>
#include <qtimer.h>

#include "Model.h"


class WorkoutPacer : public QObject {
    Q_OBJECT

public:
    explicit WorkoutPacer(const std::shared_ptr<Model> &model);

    void start();
    void stop();

    void tick() const;
private:
    std::shared_ptr<Model> model;
    QTimer timer;
};



#endif //WORKOUTPACER_H
