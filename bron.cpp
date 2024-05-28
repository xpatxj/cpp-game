#include "bron.h"

Bron::Bron(qreal x, qreal y, QGraphicsItem *parent) : QGraphicsPixmapItem(parent), xPosition(x), yPosition(y)
{
    setPos(xPosition, yPosition); // Ustawienie pozycji broni
}
