#ifndef CHARACTER_H
#define CHARACTER_H

#include <QGraphicsPixmapItem>
#include <QGraphicsItem>

class Character : public QGraphicsPixmapItem
{
public:
    Character(QGraphicsItem *parent = 0);

    void setHealth(int health) { this->health = health; }
    void setStrength(int strength) { this->strength = strength; }
    void setSpeed(int speed) { this->speed = speed; }
    // void keyPressEvent(QKeyEvent *event);
    bool czyParasol;
    int health;
    int strength;
    int speed;

private:

};

#endif // CHARACTER_H
