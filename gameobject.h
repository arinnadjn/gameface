#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QGraphicsPixmapItem>
#include <QObject>

// Абстрактный базовый класс для всех игровых объектов
class GameObject : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit GameObject(QGraphicsItem *parent = nullptr)
        : QObject(), QGraphicsPixmapItem(parent) {}
    virtual ~GameObject() = default;

    // Чисто виртуальные методы, которые должны быть реализованы в производных классах
    virtual void update() = 0; // Обновление состояния объекта
    virtual void reset() = 0;  // Сброс состояния объекта
    virtual int getType() const = 0; // Тип объекта (для идентификации)

signals:
    void objectDestroyed(GameObject* obj); // Сигнал при уничтожении объекта

protected:
    // Общие методы для всех игровых объектов
    virtual void handleCollision() = 0; // Обработка столкновений
};
#endif // GAMEOBJECT_H
