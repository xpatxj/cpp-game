#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QGraphicsScene>
#include "character.h"
#include "maincharacter.h"
#include "bron.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Character* spawnCharacter(const QString& imagePath, int x, int y, int health, int strength, int speed);
    Bron* SpawnBron(const QString& nazwa, int moc);
    QVector<Bron*> ekwipunek;
    QVector<Bron*> bronieSmoka;
    std::vector<QString> dziennik;
    QKeyEvent *event;

signals:
    void itemSelected(const QString& item);

public slots:
    void firstDialogue();
    void secondDialogue();
    void thirdDialogue();
    void finalDialogue();
    void fourthDialogue();
    void changeBackground(const QString& sceneName);
    void PassEquipment(Bron *przedmiot);
    void showEquipment();
    void showDziennik();
    void ruszajPostacia(Character *smok, Character *gracz);
    void Koniec();

private:
    Ui::MainWindow *ui;
    void saveGame();
    MainWindow *mainWindow;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *scena;
    MainCharacter *main_character;
    int counter;
    QTimer *collisionTimer;
    bool czyKoliduje;
    int xp;
    int zycie;
    int zycieWroga;
    bool czySmok;
    bool czyMożnaAtakować(int xGracza, int yGracza, int xSmoka, int ySmoka);
    Bron *wybranyPrzedmiot;
    int ileZabitych;
    int czyWszystkiePostacie;
    int silaBroni;
    Character *most;
    Character *most2;
    int X_most;
    int Y_most;
    int X_most2;
    int Y_most2;
    int liczbaDotknietychMostow;
    bool dialog;

private slots:
    void firstScene();
    void secondScene();
    void ThirdScene();
    void FourthScene();
    void FifthScene();
    void zmienPunktyZycia(int punkty);
    void zmienPunktyWroga(int punkty);
    void zmienXP(int punkty);
};
#endif // MAINWINDOW_H
