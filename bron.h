#ifndef BRON_H
#define BRON_H

#include <QString>

class Bron
{
public:
    explicit Bron(const QString& nazwa, int moc);
    QString getNazwa() const;
    int getMoc() const;

private:
    QString nazwa;
    int moc;
};

#endif // BRON_H
