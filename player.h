#ifndef PLAYER_H
#define PLAYER_H

#include "gameobject.h"
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>

class Player : public GameObject
{
    Q_OBJECT

public:
    explicit Player(QGraphicsItem *parent = nullptr);

    // Реализация чисто виртуальных методов из GameObject
    void update() override {}
    void reset() override;
    int getType() const override { return 0; } // 0 = игрок

    // Специфичные для Player методы
    void keyPressEvent(QKeyEvent *event) override;
    int getLives() const;
    void decreaseLife();
    void increaseLife();

signals:
    void lifeLost();

private:
    void handleCollision() override {}
    QPixmap createFacePixmap();

    int lives;
    int speed;
};

#endif // PLAYER_H
