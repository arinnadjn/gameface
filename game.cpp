#include "game.h"
#include "obstacle.h"
#include <QFont>
#include <QBrush>
#include <QImage>
#include <QRandomGenerator>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QPainter>
#include <QLinearGradient>
#include <QDebug>
#include <cmath>

Game::Game(QWidget *parent) : QGraphicsView(parent)
{
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 800, 600);
    setScene(scene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(800, 600);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    QPixmap bg(800, 600);
    QPainter painter(&bg);
    QLinearGradient gradient(0, 0, 0, 600);
    gradient.setColorAt(0, QColor(135, 206, 235));
    gradient.setColorAt(1, QColor(255, 255, 255));
    painter.fillRect(0, 0, 800, 600, gradient);
    scene->setBackgroundBrush(bg);

    // Игрок
    player = new Player();
    player->setPos(370, 500);
    player->setFlag(QGraphicsItem::ItemIsFocusable);
    scene->addItem(player);

    // Счёт/жизни
    score = 0;
    scoreText = new QGraphicsTextItem("Очки: 0");
    scoreText->setDefaultTextColor(Qt::blue);
    scoreText->setFont(QFont("Arial", 16, QFont::Bold));
    scoreText->setPos(10, 10);
    scene->addItem(scoreText);

    livesText = new QGraphicsTextItem("Жизни: 3");
    livesText->setDefaultTextColor(Qt::green);
    livesText->setFont(QFont("Arial", 16, QFont::Bold));
    livesText->setPos(10, 40);
    scene->addItem(livesText);

    // Таймеры
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &Game::checkCollisions);

    spawnTimer = new QTimer(this);
    connect(spawnTimer, &QTimer::timeout, this, &Game::spawnObstacle);

    difficultyTimer = new QTimer(this);
    connect(difficultyTimer, &QTimer::timeout, this, &Game::increaseDifficulty);

    playerMoveTimer = new QTimer(this);
    connect(playerMoveTimer, &QTimer::timeout, this, &Game::movePlayer);

    startGame();
}

void Game::startGame()
{
    gameTimer->start(16);
    spawnTimer->start(spawnIntervalMs);
    difficultyTimer->start(20000);
    playerMoveTimer->start(16);
}

void Game::stopGame()
{
    gameTimer->stop();
    spawnTimer->stop();
    difficultyTimer->stop();
    playerMoveTimer->stop();

    for (Obstacle *obstacle : obstacles) {
        if (obstacle) {
            obstacle->stopFalling();
        }
    }
}

void Game::resetGame()
{
    stopGame();
    removeAllObjects();

    player->reset();
    score = 0;
    scoreText->setPlainText("Очки: 0");
    livesText->setPlainText("Жизни: 3");
    livesText->setDefaultTextColor(Qt::green);

    obstacleSpeedFactor = 1.0;
    spawnIntervalMs = 800;
    spawnCount = 1;

    // Удаляем текст Game Over
    QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem *item : items) {
        QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(item);
        if (textItem && textItem != scoreText && textItem != livesText) {
            scene->removeItem(item);
            delete textItem;
        }
    }
}

void Game::spawnObject(int type)
{
    int randomX = QRandomGenerator::global()->bounded(20, 760);
    Obstacle::ObstacleType obstacleType = static_cast<Obstacle::ObstacleType>(type);

    Obstacle *obstacle = new Obstacle(obstacleType);
    obstacle->setPos(randomX, -50 - QRandomGenerator::global()->bounded(0,200));
    scene->addItem(obstacle);
    obstacles.append(obstacle);

    if (obstacleSpeedFactor != 1.0) {
        obstacle->multiplySpeed(obstacleSpeedFactor);
    }

    obstacle->startFalling();
    connect(obstacle, &Obstacle::obstacleMissed, this, &Game::handleObstacleMissed);
}

void Game::removeAllObjects()
{
    for (Obstacle *obstacle : obstacles) {
        if (obstacle) {
            obstacle->safeRemove();
        }
    }
    obstacles.clear();
}

void Game::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }

    if (event->key() == Qt::Key_Left) {
        playerDirection = -1;
    } else if (event->key() == Qt::Key_Right) {
        playerDirection = 1;
    } else if (event->key() == Qt::Key_R) {
        resetGame();
        startGame();
    }

    QGraphicsView::keyPressEvent(event);
}

void Game::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    if (event->key() == Qt::Key_Left && playerDirection == -1) {
        playerDirection = 0;
    } else if (event->key() == Qt::Key_Right && playerDirection == 1) {
        playerDirection = 0;
    }

    QGraphicsView::keyReleaseEvent(event);
}

void Game::movePlayer()
{
    if (!player) return;
    if (playerDirection == 0) return;

    qreal step = static_cast<qreal>(playerSpeed);
    qreal newX = player->x() + (playerDirection == -1 ? -step : step);
    newX = qBound(0.0, newX, 740.0);
    player->setPos(newX, player->y());
}

void Game::spawnObstacle()
{
    for (int i = 0; i < spawnCount; ++i) {
        spawnObject(QRandomGenerator::global()->bounded(1, 5));
    }
}

void Game::handleObstacleMissed(Obstacle *obstacle)
{
    if (!obstacle) return;

    if (obstacle->getObstacleType() != Obstacle::STAR) {
        score += 10;
    }
    scoreText->setPlainText("Очки: " + QString::number(score));

    if (scene && obstacle->scene()) {
        scene->removeItem(obstacle);
    }
    obstacles.removeOne(obstacle);
    obstacle->deleteLater();
}

void Game::checkCollisions()
{
    for (int i = 0; i < obstacles.size(); ++i) {
        Obstacle *obstacle = obstacles[i];

        if (!obstacle) {
            obstacles.removeAt(i);
            i--;
            continue;
        }

        if (player->collidesWithItem(obstacle)) {
            if (obstacle->getObstacleType() == Obstacle::STAR) {
                score += 50;
                scoreText->setPlainText("Очки: " + QString::number(score));
                qDebug() << "Собрана звезда! +50 очков";

            } else if (obstacle->getObstacleType() == Obstacle::HEART) {
                int currentLives = player->getLives();
                if (currentLives < 5) {
                    player->increaseLife();
                    livesText->setPlainText("Жизни: " + QString::number(player->getLives()));
                    livesText->setDefaultTextColor(Qt::green);
                    qDebug() << "Собрано сердце! +1 жизнь";
                } else {
                    score += 25;
                    scoreText->setPlainText("Очки: " + QString::number(score));
                    qDebug() << "Максимум жизней! +25 очков вместо сердца";
                }

            } else {
                player->decreaseLife();
                livesText->setPlainText("Жизни: " + QString::number(player->getLives()));
                livesText->setDefaultTextColor(player->getLives() == 1 ? Qt::red : Qt::green);
                qDebug() << "Столкновение с препятствием! -1 жизнь";
                checkGameOver();
            }

            if (scene && obstacle->scene()) scene->removeItem(obstacle);
            obstacles.removeAt(i);
            obstacle->deleteLater();
            i--;
            continue;
        }

        if (obstacle->y() > 650) {
            if (obstacles.contains(obstacle)) {
                handleObstacleMissed(obstacle);
            }
            i--;
        }
    }
}

void Game::checkGameOver()
{
    if (player->getLives() <= 0) {
        stopGame();

        QGraphicsTextItem *gameOver = new QGraphicsTextItem("ИГРА ОКОНЧЕНА!\nНажмите R для рестарта");
        gameOver->setDefaultTextColor(Qt::red);
        gameOver->setFont(QFont("Arial", 24, QFont::Bold));
        gameOver->setPos(250, 250);
        scene->addItem(gameOver);
    }
}

void Game::increaseDifficulty()
{
    const double speedFactorStep = 1.15;
    const double spawnIntervalFactor = 0.90;
    const int spawnCountStep = 1;

    obstacleSpeedFactor *= speedFactorStep;

    for (Obstacle *ob : obstacles) {
        if (ob) ob->multiplySpeed(speedFactorStep);
    }

    if (spawnCount < maxSpawnCount) spawnCount = qMin(maxSpawnCount, spawnCount + spawnCountStep);

    spawnIntervalMs = qMax(150, static_cast<int>(std::round(spawnIntervalMs * spawnIntervalFactor)));
    if (spawnTimer) spawnTimer->start(spawnIntervalMs);

    qDebug() << "Difficulty increased: speedFactor=" << obstacleSpeedFactor
             << " spawnCount=" << spawnCount << " spawnIntervalMs=" << spawnIntervalMs;
}
