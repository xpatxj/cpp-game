#ifndef CHARACTER_H
#define CHARACTER_H

#include <QGraphicsPixmapItem>
#include <QGraphicsItem>

class Character : public QGraphicsPixmapItem
{
public:
    Character(QGraphicsItem *parent = 0);

    void walk();
    void run();
    void jump();
    void fightDragon();
    void keyPressEvent(QKeyEvent *event);

private:
    int health;
    int strength;
    int speed;
};

#endif // CHARACTER_H
