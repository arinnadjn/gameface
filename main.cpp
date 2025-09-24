#include <QApplication>
#include "game.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Game game;
    game.show();
    game.setWindowTitle("Face Game - Управление стрелками ← →");

    return a.exec();
}
