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
    zycie = 1000;
    zycieWroga = 0;
    zmienPunktyZycia(0);
    zmienXP(xp);
    liczbaDotknietychMostow = 0;

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
// scena powitalna, wprowadzenie do gry
void MainWindow::firstScene()
{
    disconnect(ui->start, &QPushButton::clicked, this, &MainWindow::firstScene);
    changeBackground("wioska");

    ui->start->hide();
    ui->dialogi->show();
    ui->zycie->show();
    ui->xp->show();

    firstDialogue();

    main_character = new MainCharacter();
    scene->addItem(main_character);
    main_character->setPixmap(QPixmap(":/images/images/postac.png"));
    main_character->setPos(0, 340);

    main_character->setHealth(1000);
    main_character->setStrength(50);
    main_character->setSpeed(10);

    connect(ui->dalej, &QPushButton::clicked, this, &MainWindow::secondScene);
}

// poziom pierwszy - musimy przejść
void MainWindow::secondScene()
{
    // dezaktywuj połączenie przycisku "dalej"
    disconnect(ui->dalej, &QPushButton::clicked, this, &MainWindow::secondScene);

    ui->dalej->hide();
    ui->dialogi->clear();
    ui->ekw_but->show();
    ui->dziennik->show();
    changeBackground("tlo2");
    connect(ui->dalej, &QPushButton::clicked, this, &MainWindow::ThirdScene);

    int randomX = rand() % (720 - 300 + 1) + 200;
    int randomY = rand() % (350 - 200 + 1) + 200;

    X_most = rand() % (720 - 300 + 1) + 200;
    Y_most = rand() % (350 - 200 + 1) + 200;
    most = spawnCharacter(":/images/images/kawalek_mostu.png", X_most, Y_most, 0, 0, 0);
    most->hide();
    Character *zielony_smok = spawnCharacter(":/images/images/green_dragon2.png", 720, 290, 50, 10, 10);
    zielony_smok->hide();
    Character *npcw = spawnCharacter(":/images/images/npcw.png", randomX, randomY, 50, 0, 0);

    scene->addItem(main_character);
    main_character->setFlag(QGraphicsItem::ItemIsFocusable);
    main_character->setFocus();

    ui->dialogi->append("Podejdż do Wieśniaczki. Możesz poruszać się strzałkami.");
    ui->dialogi->append("Z każdym kolejnym poziomem zyskujesz również na szybkości.");
    dziennik.push_back("Z każdym kolejnym poziomem zyskujesz również na szybkości.");
    ui->dialogi->append("MIEJ OCZY SZEROKO OTWARTE.");
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
                ui->dialogi->append("Zrób wszystko, aby uratować ludność cywilną. Nie możesz dopuścić, aby smok zderzył się z mieszkańcem. Powodzenia");
                ui->opcja1->show();
                ui->opcja2->show();
                ui->opcja1->setText("Powiedz: Może dogadamy się pokojowo?");
                ui->opcja2->setText("Atakuj wodną bombą!");

                connect(ui->opcja1, &QPushButton::clicked, this, [=]() {
                    ui->opcja1->hide();
                    ui->dialogi->append("Ten smok nie potrafi mówić.");

                });
                // bitwa
                connect(ui->opcja2, &QPushButton::clicked, this, [=]() {
                    Bron *bombaWodna = SpawnBron("Bomba wodna", 5);
                    PassEquipment(bombaWodna);
                    wybranyPrzedmiot = bombaWodna;

                    ui->zycie_wroga->show();
                    zmienPunktyWroga(zielony_smok->health);
                    ui->opcja1->hide();
                    ui->opcja2->hide();

                    ui->dialogi->append("Atakujesz smoka!");
                    ui->dialogi->append("Rzucasz w niego wodną bombą!");
                    ui->dialogi->append("Aby atakować, kliknij spację.");
                    dziennik.push_back("Aby atakować, kliknij spację.");

                    int s = bombaWodna->getMoc();
                    zmienPunktyWroga(-s);

                    QTimer::singleShot(400, [=]() {
                        QTimer *smokTimer = new QTimer(this);
                        connect(smokTimer, &QTimer::timeout, this, [=]() {

                            most->show();
                            ruszajPostacia(zielony_smok, main_character);
                            int silaWroga = zielony_smok->strength;

                            if (main_character->collidesWithItem(zielony_smok) && zycieWroga>0) {
                                zmienPunktyZycia(-silaWroga);
                            } else if (main_character->collidesWithItem(most)) {
                                scene->removeItem(most);
                                most->hide();
                                liczbaDotknietychMostow++;
                                dziennik.push_back("Zdobądź łącznie wszystie trzy mosty, aby przejść do poziomu 3.");
                                ui->dialogi->append("Zdobądź łącznie wszystie trzy mosty, aby przejść do poziomu 3.");
                            } else if (npcw->collidesWithItem(zielony_smok) && zycieWroga>0) {
                                int npcwhealth = npcw->health;
                                npcw->setHealth(npcwhealth-silaWroga);
                                if (npcwhealth < 0) {
                                    scene->removeItem(npcw);
                                }

                            } else if (main_character->getCzyAtak()) {
                                zmienPunktyWroga(-s);
                                main_character->setCzyAtak(false);
                            } else if (zycieWroga<=0 && npcw->health>0) {
                                scene->removeItem(zielony_smok);
                                ui->dialogi->append("Pokonałeś Zielonego Smoka, a do tego udało ci się uratować Wieśniaczkę! Dostajesz 300 XP.");
                                zmienXP(300);
                                zmienPunktyZycia(100);
                                smokTimer->stop();
                                ui->zycie_wroga->hide();
                                scene->removeItem(most);
                                scene->removeItem(npcw);
                                delete npcw;
                                delete zielony_smok;
                                ui->dalej->show();
                            } else if (zycieWroga<=0 && npcw->health<0) {
                                scene->removeItem(zielony_smok);
                                scene->removeItem(most);
                                ui->dialogi->append("Pokonałeś Zielonego Smoka, ale nie udało ci się uratować Wieśniaczki. Dostajesz 100 XP.");
                                ui->dialogi->append("Pamiętaj, że to od tego, jak uratujesz miasto, będzie zależeć wynik gry.");
                                zmienXP(100);
                                zmienPunktyZycia(100);
                                ileZabitych++;
                                ui->zycie_wroga->hide();
                                smokTimer->stop();
                                delete npcw;
                                delete zielony_smok;
                                ui->dalej->show();
                            }
                        });
                        smokTimer->start(100);
                    });
                });
            });
        }
    });
    timer->start(100);
}

// spotkanie innych strażaków, trzech NPC i walka z potężniejszym smokiem, który ma szybkość 20 i moc 50, a NPC maja zycie 100
// zyskujemy dwie nowe bronie od strazakow oraz jeden od npc
void MainWindow::ThirdScene() {

    disconnect(ui->dalej, &QPushButton::clicked, this, &MainWindow::ThirdScene);
    disconnect(ui->opcja1, &QPushButton::clicked, this, nullptr);
    disconnect(ui->opcja2, &QPushButton::clicked, this, nullptr);

    ui->dalej->hide();
    ui->dialogi->clear();
    ui->ekw_but->show();
    ui->dziennik->show();
    ui->zycie_wroga->hide();
    changeBackground("tlo3");
    connect(ui->dalej, &QPushButton::clicked, this, &MainWindow::FourthScene);

    int randomX1 = rand() % 800;
    int randomY1 = rand() % 260 + 80;
    int randomX2 = rand() % 800;
    int randomY2 = rand() % 260 + 13;
    int randomX3 = rand() % 800;
    int randomY3 = rand() % 260 + 54;

    X_most = rand() % (720 - 300 + 1) + 200;
    Y_most = rand() % (350 - 200 + 1) + 200;
    X_most2 = rand() % (720 - 300 + 1) + 200;
    Y_most2 = rand() % (350 - 200 + 1) + 200;
    most = spawnCharacter(":/images/images/kawalek_mostu.png", X_most, Y_most, 0, 0, 0);
    most2 = spawnCharacter(":/images/images/kawalek_mostu.png", X_most2, Y_most2, 0, 0, 0);
    most->hide();
    most2->hide();
    Character *niebieski_smok = spawnCharacter(":/images/images/blue_dragon2.png", 720, 290, 200, 30, 10);
    niebieski_smok->hide();
    Character *npcs1 = spawnCharacter(":/images/images/npc3.png", randomX1, randomY1, 100, 5, 10);
    Character *npcw2 = spawnCharacter(":/images/images/npcw1.png", randomX2, randomY2, 100, 0, 0);
    Character *npcw3 = spawnCharacter(":/images/images/npcw2.png", randomX3, randomY3, 100, 0, 0);

    scene->removeItem(main_character);
    scene->addItem(main_character);
    main_character->setFlag(QGraphicsItem::ItemIsFocusable);
    main_character->setFocus();
    main_character->setPos(0, 340);

    czyWszystkiePostacie = 0;

    // ui->dialogi->append("<font color='red'>Podejdź do każdej z postaci, aby dowiedzieć się czegoś nowego.</font>");

    QTimer *timer3 = new QTimer(this);
    connect(timer3, &QTimer::timeout, this, [=]() {
        if (main_character->collidesWithItem(npcw3) ) {
            timer3->stop();
            czyWszystkiePostacie++;
        }
    });
    timer3->start(100);

    QTimer *timer2 = new QTimer(this);
    connect(timer2, &QTimer::timeout, this, [=]() {
        if (main_character->collidesWithItem(npcs1) ) {
            thirdDialogue();
            timer2->stop();
            czyWszystkiePostacie++;
        }
    });
    timer2->start(100);

    QTimer *timer4 = new QTimer(this);
    connect(timer4, &QTimer::timeout, this, [=]() {
        if (main_character->collidesWithItem(npcw2) ) {
            // thirdDialogue();
            timer4->stop();
            czyWszystkiePostacie++;
            ui->dialogi->append("Od teraz Twoi mieszkańcy, których musisz uratować, mają więcej życia.");
            npcw2->setHealth(npcw2->health+60);
            npcw3->setHealth(npcw3->health+60);
        }
    });
    timer4->start(100);

    QTimer *timer_wrog = new QTimer(this);
    connect(timer_wrog, &QTimer::timeout, this, [=]() {
        if (czyWszystkiePostacie == 3) {
            timer_wrog->stop();
            QTimer::singleShot(600, [=]() {
                niebieski_smok->show();
            });
            QTimer::singleShot(800, [=]() {
                ui->dialogi->append("M: To smok!");
                ui->dialogi->append("Zrób wszystko, aby uratować ludność cywilną. Nie możesz dopuścić, aby smok zderzył się z mieszkańcami. Powodzenia");
                ui->dialogi->append("Od teraz możesz wybierać bronie, którymi dysponujesz.");

                ui->opcja1->show();
                ui->opcja2->show();
                ui->opcja1->setText("Wykończcie smoka razem.");
                ui->opcja2->setText("Pokonaj go sam.");

                // opcja 1 - pokonanie z kolegą
                connect(ui->opcja1, &QPushButton::clicked, this, [=]() {
                    ui->dialogi->append("W zamian za bycie koleżeńskim, dostajesz nagrodę! 200 Punktów życia!");
                    zmienPunktyZycia(200);
                    most->show();
                    most2->show();

                    silaBroni = wybranyPrzedmiot->getMoc();
                    ui->opcja1->hide();
                    ui->opcja2->hide();

                    zycieWroga = 0;
                    zmienPunktyWroga(niebieski_smok->health);
                    ui->zycie_wroga->show();

                    int silaWroga = niebieski_smok->strength;

                    QTimer::singleShot(1000, [=]() {
                        QTimer *smokTimer = new QTimer(this);
                        connect(smokTimer, &QTimer::timeout, this, [=]() {
                            silaBroni = wybranyPrzedmiot->getMoc();
                            ruszajPostacia(niebieski_smok, main_character);
                            ruszajPostacia(npcs1, niebieski_smok);

                            if (main_character->collidesWithItem(niebieski_smok) && zycieWroga>0) {
                                zmienPunktyZycia(-silaWroga);
                            } else if (main_character->collidesWithItem(most)) {
                                scene->removeItem(most);
                                most->hide();
                                liczbaDotknietychMostow++;
                            } else if (main_character->collidesWithItem(most2)) {
                                scene->removeItem(most2);
                                most2->hide();
                                liczbaDotknietychMostow++;
                            } else if (npcs1->collidesWithItem(niebieski_smok) && zycieWroga>0){
                                zmienPunktyWroga(-5);
                                npcs1->setPos(niebieski_smok->pos().x()-220, niebieski_smok->pos().y()-140);
                            } else if (npcw2->collidesWithItem(niebieski_smok) && zycieWroga>0) {
                                int npcw2health = npcw2->health;
                                npcw2->setHealth(npcw2health-silaWroga);
                                if (npcw2health < 0) {
                                    scene->removeItem(npcw2);
                                }
                            } else if (main_character->getCzyAtak()) {
                                zmienPunktyWroga(-silaBroni);
                                main_character->setCzyAtak(false);
                            } else if (zycieWroga<=0 && npcw2->health>0 && npcw3->health>0) {
                                scene->removeItem(niebieski_smok);
                                ui->dialogi->append("Pokonałeś Niebieskiego Smoka, a do tego udało ci się uratować mieszkańców! Dostajesz 500 XP.");
                                zmienXP(500);
                                zmienPunktyZycia(200);
                                smokTimer->stop();
                                scene->removeItem(npcw2);
                                ui->zycie_wroga->hide();
                                scene->removeItem(npcw3);
                                QTimer::singleShot(1000, [=]() {
                                    ui->dialogi->append("Sam: Dzięki za współpracę! Do zobaczenia!");
                                    scene->removeItem(npcs1);
                                });

                                ui->dalej->show();
                            } else if ((zycieWroga<=0 && npcw2->health>0 && npcw3->health<=0) || (zycieWroga<=0 && npcw3->health>0 && npcw2->health<=0)) {
                                scene->removeItem(niebieski_smok);
                                ui->dialogi->append("Pokonałeś Niebieskiego Smoka, ale udało ci się uratować tylko jednego mieszkańca! Dostajesz 300 XP.");
                                zmienXP(300);
                                zmienPunktyZycia(200);

                                smokTimer->stop();
                                ileZabitych++;
                                scene->removeItem(npcw3);
                                scene->removeItem(npcw2);
                                ui->zycie_wroga->hide();
                                QTimer::singleShot(1000, [=]() {
                                    ui->dialogi->append("Sam: Dzięki za współpracę! Do zobaczenia!");
                                    scene->removeItem(npcs1);
                                });
                                ui->dalej->show();
                            } else if (zycieWroga<=0 && npcw2->health<0 && npcw3->health<0) {
                                scene->removeItem(niebieski_smok);
                                ui->dialogi->append("Pokonałeś Niebieskiego Smoka, ale nie udało ci się uratować mieszkańców. Dostajesz 100 XP.");
                                zmienXP(100);
                                zmienPunktyZycia(200);
                                ileZabitych+=2;
                                ui->zycie_wroga->hide();
                                QTimer::singleShot(1000, [=]() {
                                    ui->dialogi->append("Sam: Szkoda, że nie udało się Nam ich uratować...");
                                    scene->removeItem(npcs1);
                                });

                                smokTimer->stop();
                                ui->dalej->show();
                            }
                        });
                        smokTimer->start(100);
                    });
                });

                // opcja 2 - pokonanie samemu
                connect(ui->opcja2, &QPushButton::clicked, this, [=]() {
                    ui->dialogi->append("W zamian za bycie odważnym, dostajesz nagrodę!");
                    Bron *dynamit = SpawnBron("Dynamit", 30);
                    PassEquipment(dynamit);
                    most->show();
                    most2->show();

                    ui->opcja1->hide();
                    ui->opcja2->hide();
                    npcs1->hide();

                    zycieWroga = 0;
                    zmienPunktyWroga(niebieski_smok->health);
                    ui->zycie_wroga->show();

                    int silaWroga = niebieski_smok->strength;

                    QTimer::singleShot(1000, [=]() {
                        QTimer *smokTimer = new QTimer(this);
                        connect(smokTimer, &QTimer::timeout, this, [=]() {
                            silaBroni = wybranyPrzedmiot->getMoc();
                            ruszajPostacia(niebieski_smok, main_character);
                            if (main_character->collidesWithItem(niebieski_smok) && zycieWroga>0) {
                                zmienPunktyZycia(-silaWroga);
                            } else if (main_character->collidesWithItem(most)) {
                                most->hide();
                                scene->removeItem(most);
                                liczbaDotknietychMostow++;
                            } else if (main_character->collidesWithItem(most2)) {
                                most2->hide();
                                scene->removeItem(most2);
                                liczbaDotknietychMostow++;
                            } else if (npcw2->collidesWithItem(niebieski_smok) && zycieWroga>0) {
                                int npcw2health = npcw2->health;
                                npcw2->setHealth(npcw2health-silaWroga);
                                if (npcw2health < 0) {
                                    scene->removeItem(npcw2);
                                }
                            } else if (main_character->getCzyAtak()) {
                                zmienPunktyWroga(-silaBroni);
                                main_character->setCzyAtak(false);
                            } else if (zycieWroga<=0 && npcw2->health>0 && npcw3->health>0) {
                                scene->removeItem(niebieski_smok);
                                ui->dialogi->append("Pokonałeś Niebieskiego Smoka, a do tego udało ci się uratować mieszkańców! Dostajesz 500 XP.");
                                zmienXP(500);
                                zmienPunktyZycia(200);
                                ui->zycie_wroga->hide();
                                smokTimer->stop();
                                scene->removeItem(npcw2);
                                scene->removeItem(npcw3);
                                ui->dalej->show();
                            } else if ((zycieWroga<=0 && npcw2->health>0 && npcw3->health<=0) || (zycieWroga<=0 && npcw3->health>0 && npcw2->health<=0)) {
                                scene->removeItem(niebieski_smok);
                                ui->dialogi->append("Pokonałeś Niebieskiego Smoka, ale udało ci się uratować tylko jednego mieszkańca! Dostajesz 300 XP.");
                                zmienXP(300);
                                zmienPunktyZycia(200);

                                smokTimer->stop();
                                ui->zycie_wroga->hide();
                                ileZabitych++;
                                scene->removeItem(npcw3);
                                scene->removeItem(npcw2);
                                ui->dalej->show();
                            } else if (zycieWroga<=0 && npcw2->health<0 && npcw3->health<0) {
                                scene->removeItem(niebieski_smok);
                                ui->dialogi->append("Pokonałeś Niebieskiego Smoka, ale nie udało ci się uratować mieszkańców. Dostajesz 100 XP.");
                                zmienXP(100);
                                zmienPunktyZycia(200);

                                ui->zycie_wroga->hide();
                                ileZabitych+=2;
                                smokTimer->stop();
                                ui->dalej->show();
                            }
                        });
                        smokTimer->start(100);
                    });
                });
            });
        }
    });
    timer_wrog->start(100);
}

void MainWindow::FourthScene() {
    disconnect(ui->dalej, &QPushButton::clicked, this, &MainWindow::FourthScene);
    disconnect(ui->opcja1, &QPushButton::clicked, this, nullptr);
    disconnect(ui->opcja2, &QPushButton::clicked, this, nullptr);

    main_character->setPos(0, 340);
    ui->dialogi->clear();
    ui->dialogi->hide();
    ui->dialogi->show();
    ui->dalej->hide();
    changeBackground("final_fight");
    scene->removeItem(main_character);
    scene->addItem(main_character);

    if (liczbaDotknietychMostow >= 3) {
        ui->dialogi->append("Od tej pory możesz używać parasola, chroni cię on przed atakami.");
        main_character->setPos(10, 270);
        // main_character->setFlag(QGraphicsItem::ItemIsFocusable);
        // main_character->setFocus();
        Character *wladca_zaru = spawnCharacter(":/images/images/wladca_zaru.png", 420, 150, 2500, 0, 0);
        wladca_zaru->hide();

        QVector<Bron*> bronieSmoka = {
            new Bron("Ognisty oddech", 50),
            new Bron("Zółty deszcz", 100),
            new Bron("Mroczny pocisk", 150),
            new Bron("Błyskawica", 200),
            new Bron("Kamienne uderzenie", 500)
        };

        dialog = true;
        // QTimer *bossTimer = new QTimer(this);
        // connect(bossTimer, &QTimer::timeout, this, [=]() {
            if (dialog == true) {
                fourthDialogue();
                dialog = false;
            }

            QTimer::singleShot(600, [=]() {
                wladca_zaru->show();
                ui->zycie_wroga->show();
            });
            QTimer::singleShot(800, [=]() {

                QTimer *atakWroga = new QTimer(this);
                connect(atakWroga, &QTimer::timeout, this, [=]() {
                    silaBroni = wybranyPrzedmiot->getMoc();

                    int randomIndex = rand() % bronieSmoka.size();
                    Bron *wybranaBron = bronieSmoka[randomIndex];
                    ui->dialogi->append("Smok używa " + wybranaBron->getNazwa() + "!");
                    ui->dialogi->append(QString::number(randomIndex));

                    if (wybranyPrzedmiot->getNazwa() != "Parasol") {
                        ui->dialogi->append("Smok zaatakował cię " + wybranaBron->getNazwa() + ". Tracisz " + QString::number(wybranaBron->getMoc()) + " punktów życia.");
                        zmienPunktyZycia(-wybranaBron->getMoc());
                        if (wybranaBron->getNazwa() == "Kamienne uderzenie") {
                            ui->dialogi->append("Dostałeś bronią ostateczną!");
                        }
                    } else if (wybranyPrzedmiot->getNazwa() == "Parasol") {
                        ui->dialogi->append("Używasz Parasola, jesteś chroniony.");
                        main_character->setPixmap(QPixmap(":/images/images/postac_z_parasolem.png"));
                    } else if (main_character->getCzyAtak()) {
                        zmienPunktyWroga(-silaBroni);
                        main_character->setCzyAtak(false);
                    } else if (zycie <= 0) {
                        Koniec();
                    } else if (wladca_zaru->health <= 0 && ileZabitych < 3) {
                        wladca_zaru->hide();
                        scene->removeItem(wladca_zaru);
                        DialogWygranaGry();
                    } else if (wladca_zaru->health <= 0 || ileZabitych >= 3) {
                        DialogPrzegranaGry();
                        wladca_zaru->hide();
                        scene->removeItem(wladca_zaru);
                    }

                });
                atakWroga->start(3000);


                //Sprawdzanie warunków wygranej lub przegranej
                // if (wladca_zaru->health <= 0 && ileZabitych == 3) {
                //     wladca_zaru->hide();
                //     scene->removeItem(wladca_zaru);
                //     DialogWygranaGry();
                // } else if (wladca_zaru->health <= 0 || ileZabitych < 3) {
                //     DialogPrzegranaGry();
                //     wladca_zaru->hide();
                //     scene->removeItem(wladca_zaru);
                // }
            });
        // });
        // bossTimer->start(100);

    } else {
        ui->zycie->hide();
        ui->xp->hide();

        finalDialogue();
        connect(ui->dalej, &QPushButton::clicked, this, &MainWindow::Koniec);
    }
}

// final fight, jestesmy tylko my i boss. on moze rzucac w nas jednym z 5 broni, ktore zainicjujemy w tablicy. ma bardzo duzo zycia,
// ale my tez mamy duzo, bo zrobi nam sie x5 podczas walki z nim. on ogolnie sie nie rusza, po prostu randomowo nas atakuje.
// gdy go pokonamy, okaze sie, ze jest to stachnik xd i go "odczarujemy",
// a on bedzie mogl wrocic do swiata normalnie i rowniez stanie sie strazakiem.
// wygrywamy, jesli uratowalismy wiecej niz polowe cywili oraz pokonalismy smoka,
// przegrywamy, jesli uratowalismy mniej niz polowe cywili lub jesli nie pokonalismy smoka
void MainWindow::FifthScene() {

}

void MainWindow::PassEquipment(Bron *przedmiot) {
    ekwipunek.push_back(przedmiot);
    QString nazwa = przedmiot->getNazwa();
    ui->dialogi->append("Zdobyłeś: " + nazwa.toUpper() + ". Zobacz swój ekwipunek");
}

void MainWindow::showEquipment() {
    if(ui->ekw_list->isVisible()) {
        ui->ekw_list->hide();
    } else {
        ui->ekw_list->clear();

        if (!ekwipunek.empty()) {
            for(Bron *przedmiot : ekwipunek) {
                ui->ekw_list->addItem(przedmiot->getNazwa());
            }
        } else {
            ui->ekw_list->addItem("PUSTO");
        }
        ui->ekw_list->show();
    }
    disconnect(ui->ekw_list, &QListWidget::itemClicked, nullptr, nullptr);

    connect(ui->ekw_list, &QListWidget::itemClicked, this, [=](QListWidgetItem *item){
        for(Bron *przedmiot : ekwipunek) {
            if (przedmiot->getNazwa() == item->text()) {
                ui->dialogi->append("Wybrano przedmiot: " + przedmiot->getNazwa() + " o mocy: " + QString::number(przedmiot->getMoc()));
                // ui->dialogi->append("Moc: " + QString::number(przedmiot->getMoc()));

                wybranyPrzedmiot = przedmiot;
                break;
            }
        }
    });
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

Bron* MainWindow::SpawnBron(const QString& nazwa, int moc) {
    Bron *bron = new Bron(nazwa, moc);
    return bron;
}

bool czyMożnaAtakować(int xGracza, int yGracza, int xSmoka, int ySmoka) {
    return (qAbs(xGracza - xSmoka) >= 30 || qAbs(yGracza - ySmoka) >= 30);
}

void MainWindow::ruszajPostacia(Character *smok, Character *gracz) {
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
        Koniec();
    }
}

void MainWindow::Koniec() {
    // Usuń wszystkie elementy ze sceny
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
    ui->dialogi->hide();
    ui->zycie_wroga->hide();

    // Zmień tło na "koniec"
    changeBackground("koniec");
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
        QTimer::singleShot(1000, this, &MainWindow::firstDialogue);
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
        "W: Szybko, masz tutaj parasol!"
    };

    for (int i = 0; i < texts.size(); ++i) {


        QTimer::singleShot(100 * (i + 1), this, [=]() {
            counter++;
            ui->dialogi->append(texts[i]);
            Bron *parasol = nullptr;
            if (i == 6) {
                parasol = SpawnBron("Parasol", 0);
                PassEquipment(parasol);
            }
        });
    }
}

void MainWindow::thirdDialogue() {

    QStringList texts = {
                         "Strażak Sam: O, ty z naszych!",
                         "Sam: Mamy problem. Musimy ugasić kolejnego smoka. Podpalił kolejne miasto.",
                         "Ty: CO? Naprawdę?",
                         "Sam: Jest 300 rannych. Jeśli tak dalej pójdzie, wszyscy zginiemy!",
                         "Sam: Trzymaj się mnie, a na pewno zajdziesz daleko!",
                         };

    for (int i = 0; i < texts.size(); ++i) {

        QTimer::singleShot(100 * (i + 1), this, [=]() {
            counter++;
            ui->dialogi->append(texts[i]);
            Bron *gasnica = nullptr;
            if (i == 4) {
                gasnica = SpawnBron("Gaśnica", 25);
                PassEquipment(gasnica);
            }
        });
    }
}

void MainWindow::finalDialogue() {
    Bron *parasol = nullptr;

    QStringList texts = {
        "Niestety, nie udało Ci się zebrać całego mostu!",
        "Mowiliśmy Ci, żebyś TRZYMAŁ OCZY SZEROKO OTWARTE!",
        "Nie jesteś w stanie przejść dalej za most, który prowadziłby cię do tej przełęki.",
        "Tym samym, to nie ty pokonasz Władcę Żaru.",
        "To nie ty zgarniesz nagrodę.",
        "To nie ty uratujesz świat.",
        "Loser."
    };

    for (int i = 0; i < texts.size(); ++i) {
        QTimer::singleShot(100 * (i + 1), this, [=]() {
            counter++;
            ui->dialogi->append(texts[i]);
            if (i == 6) {
                ui->dalej->show();
            }
        });
    }
}

void MainWindow::fourthDialogue() {
    QStringList texts = {
         "Smok: Witaj, mały bohaterze. Widzę, że przyszedłeś, aby stawić mi czoła.",
         "Gracz: Tak, Smoku. Muszę Cię powstrzymać, zanim zranisz więcej ludzi.",
         "Smok: Ach, ci ludzie... Nie rozumieją, że ja również muszę przetrwać. Ale skoro przyszedłeś, przygotuj się na walkę.",
         "Gracz: Nie mam zamiaru cię oszczędzać. Muszę uratować moje miasto.",
         "Smok: Odważne słowa, mały. Zobaczymy, czy masz na tyle odwagi, aby zmierzyć się ze mną."
    };

    for (int i = 0; i < texts.size(); ++i) {
        QTimer::singleShot(100 * (i + 1), this, [=]() {
            counter++;
            ui->dialogi->append(texts[i]);

        });
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
