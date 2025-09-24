#include "player.h"
#include <QKeyEvent>
#include <QTimer>

Player::Player(QGraphicsItem *parent) : GameObject(parent)
{
    setPixmap(createFacePixmap());
    setPos(370, 500);
    lives = 3;
    speed = 10;
    setTransformOriginPoint(boundingRect().center());
    setFlag(QGraphicsItem::ItemIsFocusable);
}

QPixmap Player::createFacePixmap()
{
    QPixmap pixmap(60, 60);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Лицо
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(Qt::yellow);
    painter.drawEllipse(5, 5, 50, 50);

    // Глаза
    painter.setBrush(Qt::white);
    painter.drawEllipse(15, 20, 10, 10);
    painter.drawEllipse(35, 20, 10, 10);

    // Зрачки
    painter.setBrush(Qt::black);
    painter.drawEllipse(17, 22, 6, 6);
    painter.drawEllipse(37, 22, 6, 6);

    // Рот
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawArc(20, 35, 20, 10, 0, -180 * 16);

    return pixmap;
}

void Player::keyPressEvent(QKeyEvent *event)
{
    QPointF newPos = pos();

    if (event->key() == Qt::Key_Left) {
        newPos.setX(qMax(0.0, newPos.x() - speed));
    }
    else if (event->key() == Qt::Key_Right) {
        newPos.setX(qMin(740.0, newPos.x() + speed));
    }

    setPos(newPos);
}

int Player::getLives() const
{
    return lives;
}

void Player::decreaseLife()
{
    if (lives > 0) {
        lives--;
        emit lifeLost();

        // Анимация мигания при получении урона
        setOpacity(0.5);
        QTimer::singleShot(200, this, [this]() {
            if (this) setOpacity(1.0);
        });
    }
}

void Player::reset()
{
    setPos(370, 500);
    lives = 3;
    setOpacity(1.0);
}

void Player::increaseLife()
{
    if (lives < 5) {
        lives++;
    }
}
