#ifndef MAINCHARACTER_H
#define MAINCHARACTER_H

#include "character.h"

class MainCharacter : public Character
{
public:
    MainCharacter();

    bool getCzyAtak() const { return czyAtak; } // Getter dla czyAtak
    void setCzyAtak(bool atak) { czyAtak = atak; } // Setter dla czyAtak
    void keyPressEvent(QKeyEvent *event);

private:
    bool czyAtak; // Nowa zmienna boolowska
};

#endif // MAINCHARACTER_H
