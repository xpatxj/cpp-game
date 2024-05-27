#ifndef GAME_H
#define GAME_H

#include <QObject> // Dodajemy nagłówek QObject

class Game : public QObject // Dziedziczymy po QObject
{
    Q_OBJECT
public:
    Game();
};

#endif // GAME_H
