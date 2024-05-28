#include "character.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QGraphicsView>
#include "mainwindow.h"

Character::Character(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    health = 100;
    strength = 50;
    speed = 10;
}

