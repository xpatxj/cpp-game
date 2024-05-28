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
    ui->ekw_but->hide();
    ui->ekw_list->hide();
    ui->dziennik->hide();
    ui->zycie->hide();
    ui->dalej->hide();
    ui->xp->hide();
    ui->opcja1->hide();
    ui->opcja2->hide();
    ui->dzien_list->hide();

    xp = 0;
    zycie = 100;
    zmienPunktyZycia(0);
    zmienXP(0);

    scene = new QGraphicsScene(this);
    scena = new QGraphicsPixmapItem();

    scene->addItem(scena);
    ui->tlo->setScene(scene);
    changeBackground("start");

    connect(ui->start, &QPushButton::clicked, this, &MainWindow::firstScene);
    connect(ui->ekw_but, &QPushButton::clicked, this, &MainWindow::showEquipment);
    connect(ui->dziennik, &QPushButton::clicked, this, &MainWindow::showDziennik);

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
    PassEquipment("Kula wodna");
    ui->dalej->hide();
    ui->dialogi->clear();
    ui->ekw_but->show();
    ui->dziennik->show();
    changeBackground("tlo2");

    int randomX = rand() % (720 - 300 + 1) + 300;
    int randomY = rand() % (350 - 200 + 1) + 200;

    Character *zielony_smok = spawnCharacter(":/images/images/green_dragon2.png", 720, 290, 100, 50, 30);
    zielony_smok->hide();

    Character *npcw = spawnCharacter(":/images/images/npcw.png", randomX, randomY, 0, 0, 0);
    scene->addItem(main_character);
    main_character->setFlag(QGraphicsItem::ItemIsFocusable);
    main_character->setFocus();
    ui->dialogi->append("Podejdż do Wieśniaczki. Możesz poruszać się strzałkami.");
    ui->dialogi->append("W lewym górnym roku znajdziesz Dziennik Aktywności oraz Ekwipunek. Pomogą ci one zebrać wszystkie informacje, których się dowiedziałeś.");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        if (main_character->collidesWithItem(npcw)) {
            secondDialogue();
            timer->stop();
            QTimer::singleShot(6000, [=]() {
                zielony_smok->show();
            });
            QTimer::singleShot(8000, [=]() {
                ui->dialogi->append("Kolejny strażak...");
                ui->opcja1->show();
                ui->opcja2->show();
                ui->dialogi->append("W: Ten smok może zaatakować cię kwiatami!");
                ui->dialogi->append("Kliknij P, aby użyć parasola. Parasol działa tylko 10 sekund.");
                dziennik.push_back("Parasol działa tylko 10 sekund");
                ui->opcja1->setText("Powiedz: Może dogadamy się pokojowo?");
                ui->opcja2->setText("Atakuj!");
                connect(ui->opcja1, &QPushButton::clicked, this, [=]() {
                    ui->opcja1->hide();
                    ui->opcja2->hide();
                });
                connect(ui->opcja2, &QPushButton::clicked, this, [=]() {
                    ui->opcja1->hide();
                    ui->opcja2->hide();
                    ui->dialogi->append("Atakujesz smoka!");
                    ui->dialogi->append("Rzucasz w niego wodną bombą!");
                    QGraphicsPixmapItem *wodnaBomb = new QGraphicsPixmapItem(QPixmap(":/images/images/wodnabomba.png"));
                    wodnaBomb->setPos(zielony_smok->x() - 20, zielony_smok->y() + 20);
                    scene->addItem(wodnaBomb);

                    QTimer::singleShot(1500, [=]() {
                        scene->removeItem(wodnaBomb);
                        ui->dialogi->append("Kliknij 'K', aby uzyc bomby.");
                        dziennik.push_back("Kliknij 'K', aby uzyc bomby.");
                        QTimer *smokTimer = new QTimer(this);
                        connect(smokTimer, &QTimer::timeout, this, [=]() {
                            ruszajSmokiem(zielony_smok, main_character);

                            // Sprawdź kolizję między smokiem a graczem
                            if (main_character->collidesWithItem(zielony_smok)) {
                                // Jeśli jest kolizja, zmniejsz punkty życia gracza o 5
                                zmienPunktyZycia(-1);
                            }
                        });
                        smokTimer->start(100);

                    });
                });
            });

        } else {

        }

    });
    timer->start(100);
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
        QTimer::singleShot(100, this, &MainWindow::firstDialogue);
        break;
    case 2:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::firstDialogue);
        break;
    case 3:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::firstDialogue);
        break;
    case 4:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::firstDialogue);
        break;
    case 5:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::firstDialogue);
        break;
    case 6:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::firstDialogue);
        break;
    case 7:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::firstDialogue);
        break;
    case 8:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::firstDialogue);
        break;
    case 9:
        scene->removeItem(main_character);
        ui->dalej->show();
        counter = 0;
        break;
    default:
        counter = 0;
        break;
    }

    counter++;
}

void MainWindow::secondDialogue() {

    QStringList texts = {
        "Wieśniaczka: Nareszcie jesteś!",
        "Ty: Kim jesteś? Nic ci nie jest?",
        "W: Smoki, one znowu tu były!",
        "Ty: Jesteś jedyną osobą, która przeżyła?",
        "W: Tak... T-to... AAAAAAA!!",
        "Ty: Co się dzieje? Co się dzieje?",
        "W: Szybko, masz tutaj parasol!"};

    switch (counter) {
    case 1:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(1000, this, &MainWindow::secondDialogue);
        break;
    case 2:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(1000, this, &MainWindow::secondDialogue);
        break;
    case 3:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(1000, this, &MainWindow::secondDialogue);
        break;
    case 4:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(1000, this, &MainWindow::secondDialogue);
        break;
    case 5:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(1000, this, &MainWindow::secondDialogue);
        break;
    case 6:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(1000, this, &MainWindow::secondDialogue);
        break;
    case 7:
        ui->dialogi->append(texts[counter - 1]);
        PassEquipment("Parasol");
        ui->dialogi->append("Zdobyłeś: " + ekwipunek.back().toUpper() + ". Zobacz swój ekwipunek");
        QTimer::singleShot(100, this, &MainWindow::secondDialogue);
        break;
    case 8:
        counter = 0;
        break;
    default:
        counter = 0;
        break;
    }

    counter++;
}

// void MainWindow::thirdDialogue() {

// }

void MainWindow::PassEquipment(const QString& rzecz) {
    ekwipunek.push_back(rzecz);
}

// void MainWindow::uzycieParasola() {
//     character->setPixmap(QPixmap(imagePath));
// }

void MainWindow::showEquipment() {
    if(ui->ekw_list->isVisible()) {
        ui->ekw_list->hide();
    } else {
        ui->ekw_list->clear();

        if (!ekwipunek.empty()) {
            for(const QString& item : ekwipunek) {
                ui->ekw_list->addItem(item);
            }
        } else {
            ui->ekw_list->addItem("PUSTO");
        }
        ui->ekw_list->show();
    }
}

void MainWindow::showDziennik() {
    if(ui->dzien_list->isVisible()) {
        ui->dzien_list->hide();
    } else {
        ui->dzien_list->clear();

        if (!dziennik.empty()) {
            for(const QString& item : dziennik) {
                ui->dzien_list->addItem(item);
            }
        } else {
            ui->dzien_list->addItem("PUSTO");
        }
        ui->dzien_list->show();
    }
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

bool czyMożnaAtakować(int xGracza, int yGracza, int xSmoka, int ySmoka) {
    return (qAbs(xGracza - xSmoka) >= 30 || qAbs(yGracza - ySmoka) >= 30);
}

void MainWindow::ruszajSmokiem(Character *smok, Character *gracz) {
    qreal dx = gracz->x() - smok->x();
    qreal dy = gracz->y() - smok->y();
    qreal dlugosc = sqrt(dx*dx + dy*dy);
    dx /= dlugosc;
    dy /= dlugosc;

    qreal predkoscSmoka = 10.0;

    smok->setPos(smok->x() + predkoscSmoka * dx, smok->y() + predkoscSmoka * dy);
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

void MainWindow::zmienPunktyZycia(int punkty) {
    zycie += punkty;
    ui->zycie->setText("Życie: " + QString::number(zycie));
}

void MainWindow::zmienXP(int punkty) {
    xp += punkty;
    ui->xp->setText("XP: " + QString::number(xp));
}

MainWindow::~MainWindow()
{
    delete ui;
}
