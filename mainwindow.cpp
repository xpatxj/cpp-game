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
    ui->dialogi->hide();
    ui->ekwipunek->hide();
    ui->zycie->hide();
    ui->dalej->hide();
    ui->xp->hide();
    ui->opcja1->hide();
    ui->opcja2->hide();

    scene = new QGraphicsScene(this);
    scena = new QGraphicsPixmapItem();

    scene->addItem(scena);
    ui->tlo->setScene(scene);
    changeBackground("start");

    connect(ui->start, &QPushButton::clicked, this, &MainWindow::firstScene);

    counter = 1;
}

void MainWindow::firstScene()
{
    changeBackground("wioska");

    ui->start->hide();
    ui->dialogi->show();
    ui->zycie->show();
    ui->xp->show();

    firstDialogue();

    main_character = spawnCharacter(":/images/images/postac.png", 0, 340, 100, 100, 10);

    connect(ui->dalej, &QPushButton::clicked, this, &MainWindow::secondScene);
}

void MainWindow::secondScene()
{
    ui->dalej->hide();
    ui->dialogi->clear();
    ui->ekwipunek->show();
    changeBackground("tlo2");

    Character *zielony_smok = spawnCharacter(":/images/images/green_dragon2.png", 720, 290, 100, 50, 30);
    zielony_smok->hide();
    Character *npcw = spawnCharacter(":/images/images/npcw.png", 600, 200, 0, 0, 0);
    scene->addItem(main_character);
    main_character->setFlag(QGraphicsItem::ItemIsFocusable);
    main_character->setFocus();
    ui->dialogi->append("Podejdż do postaci. Możesz poruszać się strzałkami.");

    czyKoliduje = false;
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        if (main_character->collidesWithItem(npcw)) {
            czyKoliduje = true;
            ui->dialogi->append("Wieśniaczka: Nareszcie jesteś!!!");
            QTimer::singleShot(2000, this, [=]() {
            ui->dialogi->append("Ty: Co się stało?");
                QTimer::singleShot(2000, this, [=]() {
                    ui->dialogi->append("W: hahaha nie");
                });
            });

            timer->stop();
        } else {
            czyKoliduje = false;
        }

    });
    timer->start(100);

    // if (czyKoliduje == true) {
    //     ui->dialogi->append("Wieśniaczka: Nareszcie jesteś!!!");
    //     QTimer::singleShot(1000, this, &MainWindow::continueDisplayingText);
    //     ui->dialogi->append("Ty: Co się stało?");
    // }
}


void MainWindow::ThirdScene() {

}

void MainWindow::FourthScene() {

}

void MainWindow::FifthScene() {

}

void MainWindow::firstDialogue() {

    QStringList texts = {
        "Witaj w grze!",
        "Trafiłeś właśnie do świata smoków. Wiem, że wcale nie wyglądam jak ze świata smoków, ale przysięgam, że tak właśnie jest!",
        "Żyliśmy w normalnym, cywilizowanym państwie. Niestety, Władca Żaru, bo tak go zwiemy, powoli przejmuje władze nad miastem.",
        "Miasto było zmuszone do powołania Brygady Skrzydła Ognia, która zajmuje się zwalczaniem smoków. Niestety, im więcej smoków, tym więcej strażaków jest potrzebnych. Podjęli więc rekrutacje.",
        "Tak, brawo, to ty będziesz tym rekrutowanym strażakiem! Jesteś taki mądry.",
        "Twoje zadanie będzie względnie proste - pokonanie ich wszystkich i uratowanie ludności cywilnej. To od tego, jaki procent ludności uratujesz, zależeć będzie Twój wynik gry.",
        "Pokonaj je wszystkie, zdobywaj lepsze bronie, nagrody, rozmawiaj z jak największą ilością ludzi, aby dostać jak najwięcej wskazówek do pokonania smoków.",
        "Powodzenia, strażaku-wariacie!"
    };

    switch (counter) {
    case 1:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::continueDisplayingText);
        break;
    case 2:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::continueDisplayingText);
        break;
    case 3:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::continueDisplayingText);
        break;
    case 4:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::continueDisplayingText);
        break;
    case 5:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::continueDisplayingText);
        break;
    case 6:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::continueDisplayingText);
        break;
    case 7:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::continueDisplayingText);
        break;
    case 8:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::continueDisplayingText);
        break;
    case 9:
        scene->removeItem(main_character);
        ui->dalej->show();
        break;
    default:
        break;
    }

    counter++;
}

void MainWidnow::secondDialogue() {

}
Character* MainWindow::spawnCharacter(const QString& imagePath, int x, int y, int health, int strength, int speed)
{
    Character *character = new Character();
    scene->addItem(character);
    character->setPixmap(QPixmap(imagePath));
    character->setPos(x, y);

    character->setHealth(health);
    character->setStrength(strength);
    character->setSpeed(speed);

    return character;
}

void MainWindow::changeBackground(const QString& sceneName) {
    QString imagePath = ":/images/images/" + sceneName + ".jpg";
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        qDebug() << "Failed to load image";
    }
    else {
        scene->addPixmap(pixmap);
        scene->setSceneRect(0, 0, ui->tlo->width()-10, ui->tlo->height()-10);
    }
}
void MainWindow::continueDisplayingText() {
    firstDialogue();
}

MainWindow::~MainWindow()
{
    delete ui;
}

