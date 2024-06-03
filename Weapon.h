#ifndef WEAPON_H
#define WEAPON_H

#include <QString>

class Weapon
{
public:
    explicit Weapon(const QString& nazwa, int moc);
    QString getNazwa() const;
    int getMoc() const;

private:
    QString nazwa;
    int moc;
};

#endif // WEAPON_H
