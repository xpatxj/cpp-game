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
    std::vector<QString> dziennik;
    QKeyEvent *event;
    void handleItemSelected(const QString& item);

signals:
    void itemSelected(const QString& item);

public slots:
    void firstDialogue();
    void secondDialogue();
    void thirdDialogue();
    void changeBackground(const QString& sceneName);
    void PassEquipment(Bron *przedmiot);
    void showEquipment();
    void showDziennik();
    void ruszajSmokiem(Character *smok, Character *gracz);

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
