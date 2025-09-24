#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "gameobject.h"
#include <QTimer>

class Obstacle : public GameObject
{
    Q_OBJECT

public:
    enum ObstacleType { ROCK = 1, BOMB = 2, HEART = 3, STAR = 4 };

    explicit Obstacle(ObstacleType type, QGraphicsItem *parent = nullptr);

    // Реализация чисто виртуальных методов из GameObject
    void update() override {}
    void reset() override { stopFalling(); }
    int getType() const override { return static_cast<int>(type); }

    ObstacleType getObstacleType() const;
    void startFalling();
    void stopFalling();
    void safeRemove();
    void multiplySpeed(double factor);

signals:
    void obstacleMissed(Obstacle *self);

public slots:
    void move();

private:
    void handleCollision() override {}
    QPixmap createPixmap(ObstacleType obstacleType);

    ObstacleType type;
    int speed;
    QTimer *moveTimer;
};

#endif // OBSTACLE_H
