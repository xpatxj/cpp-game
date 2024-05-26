#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QGraphicsScene>

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

public slots:
    void displaySecondText();
    void onStartClicked();

private:
    Ui::MainWindow *ui;
    void saveGame();
    MainWindow *mainWindow;
    QGraphicsScene *scene;
};
#endif // MAINWINDOW_H
