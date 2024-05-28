#include "bron.h"

Bron::Bron(const QString& nazwa, int moc)
    : nazwa(nazwa), moc(moc)
{
}

QString Bron::getNazwa() const
{
    return nazwa;
}

int Bron::getMoc() const
{
    return moc;
}
