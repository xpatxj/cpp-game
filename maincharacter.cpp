#include "maincharacter.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QGraphicsView>

MainCharacter::MainCharacter(): Character(), czyAtak(false) // Inicjalizacja nowej zmiennej
{
    // Dodatkowa inicjalizacja lub ustawienia, jeśli są potrzebne
}

void MainCharacter::keyPressEvent(QKeyEvent *event) {

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
        this->setCzyAtak(true);
    }
    // else if (event->key() == Qt::Key_K && czyAtak) {

    // }
}
