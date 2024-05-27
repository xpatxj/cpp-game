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

public slots:
    void firstDialogue();
    void continueDisplayingText();
    void changeBackground(const QString& sceneName);

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

private slots:
    void firstScene();
    void secondScene();
    void ThirdScene();
    void FourthScene();
    void FifthScene();
};
#endif // MAINWINDOW_H
