#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QKeyEvent>
#include <QGraphicsTextItem>
#include "player.h"
#include "obstacle.h"
#include "gameobject.h"

// Интерфейс для игровой логики
class IGameLogic {
public:
    virtual ~IGameLogic() = default;
    virtual void startGame() = 0;
    virtual void stopGame() = 0;
    virtual void resetGame() = 0;
    virtual bool isGameOver() const = 0;
};

// Интерфейс для управления объектами
class IObjectManager {
public:
    virtual ~IObjectManager() = default;
    virtual void spawnObject(int type) = 0;
    virtual void removeAllObjects() = 0;
    virtual int getObjectCount() const = 0;
};

class Game : public QGraphicsView, public IGameLogic, public IObjectManager
{
    Q_OBJECT

protected:
    void showEvent(QShowEvent *event) override {
        QGraphicsView::showEvent(event);
        setFocus();
    }

public:
    explicit Game(QWidget *parent = nullptr);
    ~Game() override = default;

    // Реализация интерфейса IGameLogic
    void startGame() override;
    void stopGame() override;
    void resetGame() override;
    bool isGameOver() const override { return player->getLives() <= 0; }

    // Реализация интерфейса IObjectManager
    void spawnObject(int type) override;
    void removeAllObjects() override;
    int getObjectCount() const override { return obstacles.size(); }

    // Обработчики событий клавиатуры
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void spawnObstacle();
    void checkCollisions();
    void handleObstacleMissed(Obstacle *obstacle);
    void increaseDifficulty();
    void movePlayer();
    void checkGameOver();

private:
    QGraphicsScene *scene = nullptr;

    // Таймеры
    QTimer *gameTimer = nullptr;
    QTimer *spawnTimer = nullptr;
    QTimer *difficultyTimer = nullptr;
    QTimer *playerMoveTimer = nullptr;

    // Игровые объекты
    Player *player = nullptr;
    QGraphicsTextItem *scoreText = nullptr;
    QGraphicsTextItem *livesText = nullptr;

    // Счёт и список препятствий
    int score = 0;
    QList<Obstacle*> obstacles;

    // Параметры сложности и спавна
    double obstacleSpeedFactor = 1.0;
    int spawnIntervalMs = 800;
    int spawnCount = 1;
    int maxSpawnCount = 5;

    // Параметры управления игроком
    int playerDirection = 0;
    int playerSpeed = 8;
};

#endif // GAME_H
