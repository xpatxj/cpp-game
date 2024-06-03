#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QGraphicsScene>
#include "character.h"
#include "maincharacter.h"
#include "Weapon.h"

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
    Weapon* SpawnWeapon(const QString& nazwa, int moc);
    QVector<Weapon*> equi;
    QVector<Weapon*> dragonWeapons;
    std::vector<QString> diary;
    QKeyEvent *event;

signals:
    void itemSelected(const QString& item);

public slots:
    void FirstDialogue();
    void SecondDialogue();
    void ThirdDialogue();
    void FinalDialogue();
    void FourthDialogue();
    void WinDialogue();
    void LoseDialogue();
    void changeBackground(const QString& sceneName);
    void PassEquipment(Weapon *przedmiot);
    void ShowEquipment();
    void ShowDiary();
    void MoveCharacter(Character *smok, Character *gracz);
    void TheEnd();

private:
    Ui::MainWindow *ui;
    void saveGame();
    MainWindow *mainWindow;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *scena;
    MainCharacter *main_character;
    int counter;
    QTimer *collisionTimer;
    bool ifCollide;
    int xp;
    int hp;
    int enemyHP;
    bool ifDragon;
    bool attackPossibility(int xGracza, int yGracza, int xSmoka, int ySmoka);
    Weapon *chosenItem;
    int deadBodies;
    int ifEveryNPC;
    int weaponPower;
    Character *most;
    Character *most2;
    int X_most;
    int Y_most;
    int X_most2;
    int Y_most2;
    int touchedBridges;
    bool dialogue;

private slots:
    void FirstScene();
    void SecondScene();
    void ThirdScene();
    void FourthScene();
    void ChangeHP(int punkty);
    void ChangeEnemyHP(int punkty);
    void ChangeXP(int punkty);
};
#endif // MAINWINDOW_H
