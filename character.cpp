#include "character.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QGraphicsView>

Character::Character(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    health = 100;
    strength = 50;
    speed = 10;

    QPixmap pixmap(":/images/images/postac.png");
    pixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(pixmap);

}

void Character::keyPressEvent(QKeyEvent *event) {

    // Pobierz obszar widoku, aby ograniczyć ruch postaci do granic widoku
    QRectF viewRect = scene()->views().first()->rect();

    if (event->key() == Qt::Key_Left && x() - speed >= viewRect.left()) {
        setPos(x() - speed, y());
    }
    else if (event->key() == Qt::Key_Right && x() + pixmap().width() + speed <= viewRect.right()) {
        setPos(x() + speed, y());
    }
    else if (event->key() == Qt::Key_Up && y() - speed >= viewRect.top()) {
        setPos(x(), y() - speed);
    }
    else if (event->key() == Qt::Key_Down && y() + pixmap().height() + speed <= viewRect.bottom()) {
        setPos(x(), y() + speed);
    }
    else if (event->key() == Qt::Key_Space) {
        // Kod do ataku
    }
}

void Character::run()
{
    // Kod do biegania
}

void Character::jump()
{
    // Kod do skakania
}

void Character::fightDragon()
{
    // Kod do walki ze smokiem
}
