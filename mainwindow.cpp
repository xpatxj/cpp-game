#include "mainwindow.h"
#include "character.h"
#include "bron.h"
#include "./ui_mainwindow.h"
#include <QSettings>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QTimer>
#include <QKeyEvent>

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
    ui->zycie_wroga->hide();
    ui->dalej->hide();
    ui->xp->hide();
    ui->opcja1->hide();
    ui->opcja2->hide();
    ui->dzien_list->hide();

    xp = 0;
    zycie = 100;
    zycieWroga = 0;
    zmienPunktyZycia(0);

    zmienXP(0);

    scene = new QGraphicsScene(this);
    scena = new QGraphicsPixmapItem();

    scene->addItem(scena);
    ui->tlo->setScene(scene);
    changeBackground("start");

    connect(ui->start, &QPushButton::clicked, this, &MainWindow::firstScene);
    connect(ui->ekw_but, &QPushButton::clicked, this, &MainWindow::showEquipment);
    connect(this, &MainWindow::itemSelected, this, &MainWindow::handleItemSelected);
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

    main_character = spawnCharacter(":/images/images/postac.png", 0, 340, 100, 50, 10);

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

    Character *zielony_smok = spawnCharacter(":/images/images/green_dragon2.png", 720, 290, 50, 1, 10);
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
            QTimer::singleShot(600, [=]() {
                zielony_smok->show();
            });
            QTimer::singleShot(800, [=]() {
                ui->dialogi->append("Smok: Kolejny strażak...");
                ui->opcja1->show();
                ui->opcja2->show();
                ui->opcja1->setText("Powiedz: Może dogadamy się pokojowo?");
                ui->opcja2->setText("Atakuj wodną bombą!");
                connect(ui->opcja1, &QPushButton::clicked, this, [=]() {
                    ui->opcja1->hide();
                    ui->opcja2->hide();
                    ui->dialogi->append("Smoki nie potrafią mówić.");
                    dziennik.push_back("Smoki nie potrafią mówić.");

                });
                // bitwa
                connect(ui->opcja2, &QPushButton::clicked, this, [=]() {
                    ui->zycie_wroga->show();
                    zmienPunktyWroga(zielony_smok->health);
                    ui->opcja1->hide();
                    // ui->opcja2->hide();

                    ui->dialogi->append("Atakujesz smoka!");
                    ui->dialogi->append("Rzucasz w niego wodną bombą!");
                    int s = main_character->strength;
                    zmienPunktyWroga(-s);

                    QTimer::singleShot(4000, [=]() {
                        QTimer *smokTimer = new QTimer(this);
                        connect(smokTimer, &QTimer::timeout, this, [=]() {
                            ruszajSmokiem(zielony_smok, main_character);

                            if (main_character->collidesWithItem(zielony_smok) && zycieWroga>0) {
                                int sila = zielony_smok->strength;
                                zmienPunktyZycia(-sila);
                            } else if (zycieWroga<=0) {
                                scene->removeItem(zielony_smok);
                                ui->dialogi->append("Pokonałeś smoka!");
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

    // Po wybraniu przedmiotu z listy "ekwipunek"
    connect(ui->ekw_list, &QListWidget::itemClicked, this, [=](QListWidgetItem *item){
        emit itemSelected(item->text());
    });
}

void MainWindow::handleItemSelected(const QString& item) {
    wybranyPrzedmiot = item;
    ui->dialogi->append("Wybrano przedmiot: " + wybranyPrzedmiot);
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

    // Debugowanie: Wyświetlenie współrzędnych x i y postaci oraz jej stanu widoczności
    qDebug() << "Character position (x, y):" << character->x() << "," << character->y();
    qDebug() << "Character visibility:" << character->isVisible();

    return character;
}

Bron* MainWindow::SpawnBron(Character *character, const QString& imagePath, char status) {
    Bron *bron = new Bron(character->x(), character->y());
    scene->addItem(bron);
    bron->setPos(character->x(), character->y());
    bron->setPixmap(QPixmap(imagePath));
    bron->show();
    // Debugowanie: Wyświetlenie współrzędnych x i y postaci oraz jej stanu widoczności
    qDebug() << "Bron position (x, y):" << character->x() << "," << character->y();
    qDebug() << "Bron visibility:" << character->isVisible();

    return bron;
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

    qreal predkoscSmoka = smok->speed;

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
    if (zycie > 0) {
        zycie += punkty;
        ui->zycie->setText("Życie: " + QString::number(zycie));
    }
    else {
        // Usuń wszystkie elementy ze sceny
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
        ui->dialogi->hide();

        // Zmień tło na "koniec"
        changeBackground("koniec");
    }
}

void MainWindow::zmienPunktyWroga(int punkty) {
    zycieWroga += punkty;
    ui->zycie_wroga->setText("Życie wroga: " + QString::number(zycieWroga));
}
void MainWindow::zmienXP(int punkty) {
    xp += punkty;
    ui->xp->setText("XP: " + QString::number(xp));
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
        QTimer::singleShot(100, this, &MainWindow::secondDialogue);
        break;
    case 2:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::secondDialogue);
        break;
    case 3:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::secondDialogue);
        break;
    case 4:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::secondDialogue);
        break;
    case 5:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::secondDialogue);
        break;
    case 6:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::secondDialogue);
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

MainWindow::~MainWindow()
{
    delete ui;
}
