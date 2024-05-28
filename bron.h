#ifndef BRON_H
#define BRON_H

#include <QGraphicsPixmapItem>
#include <QObject>

class Bron : public QGraphicsPixmapItem
{
private:
    qreal xPosition; // Pozycja x broni
    qreal yPosition; // Pozycja y broni

public:
    Bron(qreal x, qreal y, QGraphicsItem *parent = nullptr);
};

#endif // BRON_H
