#include "Weapon.h"

Weapon::Weapon(const QString& nazwa, int moc)
    : nazwa(nazwa), moc(moc)
{
}

QString Weapon::getNazwa() const
{
    return nazwa;
}

int Weapon::getMoc() const
{
    return moc;
}
