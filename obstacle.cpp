#include "obstacle.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QGraphicsScene>
#include <QTimer>
#include <QDebug>
#include <cmath>
#include <QLinearGradient>
#include <QRadialGradient>

Obstacle::Obstacle(ObstacleType type, QGraphicsItem *parent)
    : GameObject(parent), type(type)
{
    setPixmap(createPixmap(type));
    speed = QRandomGenerator::global()->bounded(3, 8);
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Obstacle::move);
}

QPixmap Obstacle::createPixmap(ObstacleType obstacleType)
{
    QPixmap pixmap(50, 50);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    switch (obstacleType) {
    case ROCK: {
        QLinearGradient rockGradient(5, 5, 35, 35);
        rockGradient.setColorAt(0, QColor(120, 120, 120));
        rockGradient.setColorAt(0.5, QColor(80, 80, 80));
        rockGradient.setColorAt(1, QColor(50, 50, 50));

        painter.setPen(QPen(QColor(40, 40, 40), 2));
        painter.setBrush(rockGradient);

        QPainterPath rockPath;
        rockPath.moveTo(15, 5);
        rockPath.cubicTo(25, 2, 35, 8, 40, 15);
        rockPath.cubicTo(45, 25, 40, 35, 30, 40);
        rockPath.cubicTo(20, 45, 10, 40, 5, 30);
        rockPath.cubicTo(2, 20, 8, 10, 15, 5);
        painter.drawPath(rockPath);

        painter.setPen(QPen(QColor(60, 60, 60), 1));
        painter.drawLine(12, 12, 18, 18);
        painter.drawLine(25, 8, 30, 13);
        painter.drawLine(35, 20, 40, 25);
        painter.drawLine(15, 30, 20, 35);
        break;
    }

    case BOMB: {
        QLinearGradient bombGradient(10, 5, 30, 35);
        bombGradient.setColorAt(0, QColor(80, 80, 80));
        bombGradient.setColorAt(1, QColor(40, 40, 40));

        painter.setPen(QPen(QColor(30, 30, 30), 2));
        painter.setBrush(bombGradient);
        painter.drawEllipse(10, 10, 30, 30);

        painter.setPen(QPen(QColor(139, 69, 19), 3));
        painter.drawLine(25, 5, 25, 10);

        QRadialGradient fireGradient(25, 8, 5);
        fireGradient.setColorAt(0, QColor(255, 255, 0));
        fireGradient.setColorAt(0.7, QColor(255, 165, 0));
        fireGradient.setColorAt(1, QColor(255, 0, 0));

        painter.setPen(Qt::NoPen);
        painter.setBrush(fireGradient);

        QPainterPath flamePath;
        flamePath.moveTo(25, 3);
        flamePath.cubicTo(28, 0, 32, 2, 33, 5);
        flamePath.cubicTo(32, 8, 28, 10, 25, 8);
        flamePath.cubicTo(22, 10, 18, 8, 17, 5);
        flamePath.cubicTo(18, 2, 22, 0, 25, 3);
        painter.drawPath(flamePath);

        painter.setPen(QPen(Qt::red, 1));
        painter.setFont(QFont("Arial", 8, QFont::Bold));
        painter.drawText(15, 25, "BOMB");
        break;
    }

    case HEART: {
        QRadialGradient heartGradient(25, 20, 20);
        heartGradient.setColorAt(0, QColor(255, 105, 97));
        heartGradient.setColorAt(0.7, QColor(220, 20, 60));
        heartGradient.setColorAt(1, QColor(178, 34, 34));

        painter.setPen(QPen(QColor(139, 0, 0), 2));
        painter.setBrush(heartGradient);

        QPainterPath heartPath;
        heartPath.moveTo(25, 35);
        heartPath.cubicTo(15, 30, 5, 20, 10, 10);
        heartPath.cubicTo(15, 5, 20, 8, 25, 15);
        heartPath.cubicTo(30, 8, 35, 5, 40, 10);
        heartPath.cubicTo(45, 20, 35, 30, 25, 35);
        painter.drawPath(heartPath);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 150));
        painter.drawEllipse(18, 12, 6, 6);
        break;
    }

    case STAR: {
        QRadialGradient starGradient(25, 25, 20);
        starGradient.setColorAt(0, QColor(255, 255, 200));
        starGradient.setColorAt(0.5, QColor(255, 255, 0));
        starGradient.setColorAt(1, QColor(255, 215, 0));

        painter.setPen(QPen(QColor(218, 165, 32), 2));
        painter.setBrush(starGradient);

        QPointF starPoints[10];
        for (int i = 0; i < 10; ++i) {
            double angle = M_PI * i / 5;
            double radius = (i % 2 == 0) ? 20.0 : 10.0;
            starPoints[i] = QPointF(25 + radius * sin(angle),
                                    25 - radius * cos(angle));
        }
        painter.drawPolygon(starPoints, 10);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 200, 100));
        for (int i = 0; i < 10; ++i) {
            double angle = M_PI * i / 5;
            double radius = 8.0;
            QPointF center(25 + radius * sin(angle),
                           25 - radius * cos(angle));
            painter.drawEllipse(center, 3, 3);
        }

        painter.setBrush(QColor(255, 255, 255, 200));
        painter.drawEllipse(22, 22, 6, 6);
        break;
    }
    }

    painter.setPen(QPen(QColor(0, 0, 0, 80), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(2, 42, 46, 4);

    return pixmap;
}

Obstacle::ObstacleType Obstacle::getObstacleType() const
{
    return type;
}

void Obstacle::startFalling()
{
    if (moveTimer) {
        moveTimer->start(16);
    }
}

void Obstacle::stopFalling()
{
    if (moveTimer && moveTimer->isActive()) {
        moveTimer->stop();
    }
}

void Obstacle::move()
{
    if (!moveTimer) return;

    setPos(x(), y() + speed);

    if (y() > 600) {
        stopFalling();
        emit obstacleMissed(this);
    }
}

void Obstacle::safeRemove()
{
    stopFalling();
    if (scene()) {
        scene()->removeItem(this);
    }
    deleteLater();
}

void Obstacle::multiplySpeed(double factor)
{
    if (factor <= 0.0) return;
    int newSpeed = qMax(1, static_cast<int>(std::round(speed * factor)));
    speed = newSpeed;
}
