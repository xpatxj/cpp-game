#include "mainwindow.h"
#include "character.h"
#include "./ui_mainwindow.h"
#include <QSettings>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textBrowser->hide();
    ui->ekwipunek->hide();
    ui->dalej->hide();

    scene = new QGraphicsScene(this);

    QPixmap pixmap(":/images/images/start.jpg");
    if (pixmap.isNull()) {
        qDebug() << "Failed to load image";
    }

    QGraphicsPixmapItem *scena = new QGraphicsPixmapItem(pixmap);
    scene->addItem(scena);
    ui->tlo->setScene(scene);
    scene->setSceneRect(0, 0, ui->tlo->width()-10, ui->tlo->height()-10);
    scena->setPixmap(pixmap.scaled(ui->tlo->size(), Qt::KeepAspectRatio));
    ui->tlo->viewport()->update();

    connect(ui->start, &QPushButton::clicked, this, &MainWindow::onStartClicked);

}

void MainWindow::onStartClicked()
{
    QPixmap pixmap(":/images/images/wioska.jpg");
    scene->addPixmap(pixmap);
    ui->tlo->setScene(scene);

    ui->start->hide();
    ui->textBrowser->show();

    ui->textBrowser->setText("Witaj w grze!");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::displaySecondText);
    timer->start(4000);

    Character *character = new Character();
    scene->addItem(character);
    character->setPos(0, 340); // ustawia pozycje postaci
    character->setFlag(QGraphicsItem::ItemIsFocusable);
    character->setFocus();
}

void MainWindow::displaySecondText() {
    static int counter = 0;
    counter++;
    if(ui->textBrowser->underMouse()){ // Sprawdzenie, czy QTextBrowser zostało kliknięte
        counter++;
    }

    // ui->textBrowser->clear();

    if (counter == 1) {
        ui->textBrowser->append("Trafiłeś właśnie do świata smoków. Wiem, że wcale nie wyglądam jak ze świata smoków, ale przysięgam, że tak właśnie jest!");
    } else if (counter == 2) {
        ui->textBrowser->append("Żyliśmy w normalnym, cywilizowanym państwie. Niestety, Władca Żaru, bo tak go zwiemy, powoli przejmuje władze nad miastem.");
    } else if (counter == 3) {
        ui->textBrowser->append("Miasto było zmuszone do powołania Brygady Skrzydła Ognia, która zajmuje się zwalczaniem smoków. Niestety, im więcej smoków, tym więcej strażaków jest potrzebnych. Podjęli więc rekrutacje.");
    } else if (counter == 4){
        ui->textBrowser->clear();
        ui->textBrowser->append("Tak, brawo, to ty będziesz tym rekrutowanym strażakiem! Jesteś taki mądry.");
    } else if (counter == 5){
        ui->textBrowser->append("Twoje zadanie będzie względnie proste - pokonanie ich wszystkich i uratowanie ludności cywilnej. To od tego, jaki procent ludności uratujesz, zależeć będzie Twój wynik gry.");
    } else if (counter == 6){
    ui->textBrowser->append("Pokonaj je wszystkie, zdobywaj lepsze bronie, nagrody, rozmawiaj z jak największą ilością ludzi, aby dostać jak najwięcej wskazówek do pokonania smoków.");
    } else if (counter == 7){
        ui->textBrowser->clear();
        ui->textBrowser->append("Powodzenia, strażaku-wariacie!");
    } else {
        ui->dalej->show();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

