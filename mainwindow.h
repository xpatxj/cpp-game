#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QGraphicsScene>
#include "character.h"

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
    std::vector<QString> ekwipunek;
    std::vector<QString> dziennik;

public slots:
    void firstDialogue();
    void secondDialogue();
    void changeBackground(const QString& sceneName);
    void PassEquipment(const QString& rzecz);
    void showEquipment();
    void showDziennik();
    void ruszajSmokiem(Character *smok, Character *gracz);

private:
    Ui::MainWindow *ui;
    void saveGame();
    MainWindow *mainWindow;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *scena;
    Character *main_character;
    int counter;
    QTimer *collisionTimer;
    bool czyKoliduje;
    int xp;
    int zycie;
    bool czySmok;
    bool czyMożnaAtakować(int xGracza, int yGracza, int xSmoka, int ySmoka);

private slots:
    void firstScene();
    void secondScene();
    void ThirdScene();
    void FourthScene();
    void FifthScene();
    void zmienPunktyZycia(int punkty);
    void zmienXP(int punkty);
};
#endif // MAINWINDOW_H
