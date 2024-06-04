#include "mainwindow.h"
#include "character.h"
#include "Weapon.h"
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
    // ustawiamy wszelkie elementy na ekranie na sam start
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
    hp = 1000;
    enemyHP = 0;
    ChangeHP(0);
    ChangeXP(xp);
    touchedBridges = 0;

    scene = new QGraphicsScene(this);
    scena = new QGraphicsPixmapItem();

    scene->addItem(scena);
    ui->tlo->setScene(scene);
    changeBackground("start");

    connect(ui->start, &QPushButton::clicked, this, &MainWindow::FirstScene);
    connect(ui->ekw_but, &QPushButton::clicked, this, &MainWindow::ShowEquipment);
    connect(ui->dziennik, &QPushButton::clicked, this, &MainWindow::ShowDiary);
    // QObject::connect(ui->menuWyjdz, &QAction::triggered, this, &MainWindow::exitGame);
    // connect(ui->exitGame_2, &QAction::triggered, this, &MainWindow::exitGame);


    counter = 1;
}

// scena powitalna, wprowadzenie do gry
void MainWindow::FirstScene()
{
    // dezaktywuj połączenie przycisku "start", ponieważ nie jest już potrzebny
    disconnect(ui->start, &QPushButton::clicked, this, &MainWindow::FirstScene);
    changeBackground("wioska");

    ui->start->hide();
    ui->dialogi->show();
    ui->zycie->show();
    ui->xp->show();

    FirstDialogue();

    main_character = new MainCharacter();
    scene->addItem(main_character);
    main_character->setPixmap(QPixmap(":/images/images/postac.png"));
    main_character->setPos(0, 340);

    main_character->setHealth(1000);
    main_character->setStrength(50);
    main_character->setSpeed(10);

    connect(ui->dalej, &QPushButton::clicked, this, &MainWindow::SecondScene);
}

// poziom pierwszy - musimy przejść
void MainWindow::SecondScene()
{
    // dezaktywuj połączenie przycisku "dalej"
    disconnect(ui->dalej, &QPushButton::clicked, this, &MainWindow::SecondScene);

    ui->dalej->hide();
    ui->dialogi->clear();
    ui->ekw_but->show();
    ui->dziennik->show();
    changeBackground("tlo2"); // zmień tło na tło drugiego poziomu
    connect(ui->dalej, &QPushButton::clicked, this, &MainWindow::ThirdScene);

    int randomX = rand() % (720 - 300 + 1) + 200; // losowe współrzędne X dla npc
    int randomY = rand() % (350 - 200 + 1) + 200; // losowe współrzędne Y dla npc

    X_most = rand() % (720 - 300 + 1) + 200;
    Y_most = rand() % (350 - 200 + 1) + 200;

    // spawnujemy postacie
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
    diary.push_back("Z każdym kolejnym poziomem zyskujesz również na szybkości.");
    ui->dialogi->append("MIEJ OCZY SZEROKO OTWARTE.");
    ui->dialogi->append("W lewym górnym roku znajdziesz Dziennik Aktywności oraz Ekwipunek. Pomogą ci one zebrać wszystkie informacje, których się dowiedziałeś.");

    // używamy timerów, aby sprawdzić, czy gracz zderzył się z NPC
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        if (main_character->collidesWithItem(npcw)) {
            SecondDialogue();
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
                // bitwa z pierwszym smokiem
                connect(ui->opcja2, &QPushButton::clicked, this, [=]() {
                    Weapon *bombaWodna = SpawnWeapon("Bomba wodna", 5);
                    PassEquipment(bombaWodna);
                    chosenItem = bombaWodna;

                    ui->zycie_wroga->show();
                    ChangeEnemyHP(zielony_smok->health);
                    ui->opcja1->hide();
                    ui->opcja2->hide();

                    ui->dialogi->append("Atakujesz smoka!");
                    ui->dialogi->append("Rzucasz w niego wodną bombą!");
                    ui->dialogi->append("Aby atakować, kliknij spację.");
                    diary.push_back("Aby atakować, kliknij spację.");

                    int s = bombaWodna->getMoc();
                    ChangeEnemyHP(-s);

                    QTimer::singleShot(400, [=]() {
                        QTimer *smokTimer = new QTimer(this);
                        connect(smokTimer, &QTimer::timeout, this, [=]() {

                            most->show();
                            MoveCharacter(zielony_smok, main_character);
                            int silaWroga = zielony_smok->strength;

                            if (main_character->collidesWithItem(zielony_smok) && enemyHP>0) {
                                ChangeHP(-silaWroga);
                            } else if (main_character->collidesWithItem(most)) {
                                scene->removeItem(most);
                                most->hide();
                                touchedBridges++; // zwiększamy liczbę mostów, które gracz zdobył
                                diary.push_back("Zdobądź łącznie wszystie trzy mosty, aby przejść do poziomu 3.");
                                ui->dialogi->append("Zdobądź łącznie wszystie trzy mosty, aby przejść do poziomu 3.");
                            } else if (npcw->collidesWithItem(zielony_smok) && enemyHP>0) {
                                int npcwhealth = npcw->health;
                                npcw->setHealth(npcwhealth-silaWroga);
                                if (npcwhealth < 0) {
                                    scene->removeItem(npcw);
                                }

                            } else if (main_character->getCzyAtak()) {
                                ChangeEnemyHP(-s);
                                main_character->setCzyAtak(false);
                            } else if (enemyHP<=0 && npcw->health>0) {
                                scene->removeItem(zielony_smok);
                                ui->dialogi->append("Pokonałeś Zielonego Smoka, a do tego udało ci się uratować Wieśniaczkę! Dostajesz 300 XP."); // dodajemy XP
                                ChangeXP(300);
                                ChangeHP(100);
                                smokTimer->stop();
                                ui->zycie_wroga->hide();
                                scene->removeItem(most);
                                scene->removeItem(npcw);
                                delete npcw;
                                delete zielony_smok;
                                ui->dalej->show();
                            } else if (enemyHP<=0 && npcw->health<0) {
                                scene->removeItem(zielony_smok);
                                scene->removeItem(most);
                                ui->dialogi->append("Pokonałeś Zielonego Smoka, ale nie udało ci się uratować Wieśniaczki. Dostajesz 100 XP.");
                                ui->dialogi->append("Pamiętaj, że to od tego, jak uratujesz miasto, będzie zależeć wynik gry.");
                                ChangeXP(100);
                                ChangeHP(100);
                                deadBodies++;
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

// spotkanie innyego strażaka oraz kolejnych dwóch mieszkańców,
// którzy pomogą nam w walce z smokiem
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

    ifEveryNPC = 0;

    // sprawdzamy czy gracz zderzył się z NPC
    QTimer *timer3 = new QTimer(this);
    connect(timer3, &QTimer::timeout, this, [=]() {
        if (main_character->collidesWithItem(npcw3) ) {
            timer3->stop();
            ifEveryNPC++;
        }
    });
    timer3->start(100);

    QTimer *timer2 = new QTimer(this);
    connect(timer2, &QTimer::timeout, this, [=]() {
        if (main_character->collidesWithItem(npcs1) ) {
            ThirdDialogue();
            timer2->stop();
            ifEveryNPC++;
        }
    });
    timer2->start(100);

    QTimer *timer4 = new QTimer(this);
    connect(timer4, &QTimer::timeout, this, [=]() {
        if (main_character->collidesWithItem(npcw2) ) {
            // thirdDialogue();
            timer4->stop();
            ifEveryNPC++;
            ui->dialogi->append("Od teraz Twoi mieszkańcy, których musisz uratować, mają więcej życia.");
            npcw2->setHealth(npcw2->health+60);
            npcw3->setHealth(npcw3->health+60);
        }
    });
    timer4->start(100);

    // musi zderzyć się z każdym, aby rozpocząć walkę z smokiem
    QTimer *timer_wrog = new QTimer(this);
    connect(timer_wrog, &QTimer::timeout, this, [=]() {
        if (ifEveryNPC == 3) {
            timer_wrog->stop();
            QTimer::singleShot(600, [=]() {
                niebieski_smok->show();
            });
            QTimer::singleShot(800, [=]() {
                ui->dialogi->append("M: To smok!");
                ui->dialogi->append("Zrób wszystko, aby uratować ludność cywilną. Nie możesz dopuścić, aby smok zderzył się z mieszkańcami. Powodzenia");
                ui->dialogi->append("Od teraz możesz wybierać bronie, którymi dysponujesz.");

                // znowu dwie opcje do wyboru
                ui->opcja1->show();
                ui->opcja2->show();
                ui->opcja1->setText("Wykończcie smoka razem.");
                ui->opcja2->setText("Pokonaj go sam.");

                // opcja 1 - pokonanie z kolegą
                connect(ui->opcja1, &QPushButton::clicked, this, [=]() {
                    ui->dialogi->append("W zamian za bycie koleżeńskim, dostajesz nagrodę! 200 Punktów życia!");
                    ChangeHP(200);
                    most->show();
                    most2->show();

                    weaponPower = chosenItem->getMoc();
                    ui->opcja1->hide();
                    ui->opcja2->hide();

                    enemyHP = 0;
                    ChangeEnemyHP(niebieski_smok->health);
                    ui->zycie_wroga->show();

                    int silaWroga = niebieski_smok->strength;

                    QTimer::singleShot(1000, [=]() {
                        QTimer *smokTimer = new QTimer(this);
                        connect(smokTimer, &QTimer::timeout, this, [=]() {
                            weaponPower = chosenItem->getMoc();
                            MoveCharacter(niebieski_smok, main_character);
                            MoveCharacter(npcs1, niebieski_smok);

                            if (main_character->collidesWithItem(niebieski_smok) && enemyHP>0) {
                                ChangeHP(-silaWroga);
                                
                            } else if (main_character->collidesWithItem(most)) { // zderzenie z mostem
                                scene->removeItem(most);
                                most->hide();
                                touchedBridges++;
                            } else if (main_character->collidesWithItem(most2)) {
                                scene->removeItem(most2);
                                most2->hide();
                                touchedBridges++;
                            } else if (npcs1->collidesWithItem(niebieski_smok) && enemyHP>0){ // zderzenie z NPC
                                ChangeEnemyHP(-5);
                                npcs1->setPos(niebieski_smok->pos().x()-220, niebieski_smok->pos().y()-140);
                            } else if (npcw2->collidesWithItem(niebieski_smok) && enemyHP>0) { // zderzenie ze smokiem
                                int npcw2health = npcw2->health;
                                npcw2->setHealth(npcw2health-silaWroga);
                                if (npcw2health < 0) {
                                    scene->removeItem(npcw2);
                                }
                            } else if (main_character->getCzyAtak()) { // atak gracza
                                ChangeEnemyHP(-weaponPower);
                                main_character->setCzyAtak(false);
                            } else if (enemyHP<=0 && npcw2->health>0 && npcw3->health>0) { // wygrana
                                scene->removeItem(niebieski_smok);
                                ui->dialogi->append("Pokonałeś Niebieskiego Smoka, a do tego udało ci się uratować mieszkańców! Dostajesz 500 XP.");
                                ChangeXP(500);
                                ChangeHP(200);
                                smokTimer->stop();
                                scene->removeItem(npcw2);
                                ui->zycie_wroga->hide();
                                scene->removeItem(npcw3);
                                QTimer::singleShot(1000, [=]() {
                                    ui->dialogi->append("Sam: Dzięki za współpracę! Do zobaczenia!");
                                    scene->removeItem(npcs1);
                                });

                                ui->dalej->show();
                            } else if ((enemyHP<=0 && npcw2->health>0 && npcw3->health<=0) || (enemyHP<=0 && npcw3->health>0 && npcw2->health<=0)) { // wygrana z jednym NPC
                                scene->removeItem(niebieski_smok);
                                ui->dialogi->append("Pokonałeś Niebieskiego Smoka, ale udało ci się uratować tylko jednego mieszkańca! Dostajesz 300 XP.");
                                ChangeXP(300);
                                ChangeHP(200);

                                smokTimer->stop();
                                deadBodies++;
                                scene->removeItem(npcw3);
                                scene->removeItem(npcw2);
                                ui->zycie_wroga->hide();
                                QTimer::singleShot(1000, [=]() {
                                    ui->dialogi->append("Sam: Dzięki za współpracę! Do zobaczenia!");
                                    scene->removeItem(npcs1);
                                });
                                ui->dalej->show();
                            } else if (enemyHP<=0 && npcw2->health<0 && npcw3->health<0) { // przegrana
                                scene->removeItem(niebieski_smok);
                                ui->dialogi->append("Pokonałeś Niebieskiego Smoka, ale nie udało ci się uratować mieszkańców. Dostajesz 100 XP.");
                                ChangeXP(100);
                                ChangeHP(200);
                                deadBodies+=2;
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
                    Weapon *dynamit = SpawnWeapon("Dynamit", 30);
                    PassEquipment(dynamit);
                    most->show();
                    most2->show();

                    ui->opcja1->hide();
                    ui->opcja2->hide();
                    npcs1->hide();

                    enemyHP = 0;
                    ChangeEnemyHP(niebieski_smok->health);
                    ui->zycie_wroga->show();

                    int silaWroga = niebieski_smok->strength;

                    QTimer::singleShot(1000, [=]() {
                        QTimer *smokTimer = new QTimer(this);
                        connect(smokTimer, &QTimer::timeout, this, [=]() {
                            weaponPower = chosenItem->getMoc();
                            MoveCharacter(niebieski_smok, main_character);
                            if (main_character->collidesWithItem(niebieski_smok) && enemyHP>0) {
                                ChangeHP(-silaWroga);
                            } else if (main_character->collidesWithItem(most)) {
                                most->hide();
                                scene->removeItem(most);
                                touchedBridges++;
                            } else if (main_character->collidesWithItem(most2)) {
                                most2->hide();
                                scene->removeItem(most2);
                                touchedBridges++;
                            } else if (npcw2->collidesWithItem(niebieski_smok) && enemyHP>0) {
                                int npcw2health = npcw2->health;
                                npcw2->setHealth(npcw2health-silaWroga);
                                if (npcw2health < 0) {
                                    scene->removeItem(npcw2);
                                }
                            } else if (main_character->getCzyAtak()) {
                                ChangeEnemyHP(-weaponPower);
                                main_character->setCzyAtak(false);
                            } else if (enemyHP<=0 && npcw2->health>0 && npcw3->health>0) {
                                scene->removeItem(niebieski_smok);
                                ui->dialogi->append("Pokonałeś Niebieskiego Smoka, a do tego udało ci się uratować mieszkańców! Dostajesz 500 XP.");
                                ChangeXP(500);
                                ChangeHP(200);
                                ui->zycie_wroga->hide();
                                smokTimer->stop();
                                scene->removeItem(npcw2);
                                scene->removeItem(npcw3);
                                ui->dalej->show();
                            } else if ((enemyHP<=0 && npcw2->health>0 && npcw3->health<=0) || (enemyHP<=0 && npcw3->health>0 && npcw2->health<=0)) {
                                scene->removeItem(niebieski_smok);
                                ui->dialogi->append("Pokonałeś Niebieskiego Smoka, ale udało ci się uratować tylko jednego mieszkańca! Dostajesz 300 XP.");
                                ChangeXP(300);
                                ChangeHP(200);

                                smokTimer->stop();
                                ui->zycie_wroga->hide();
                                deadBodies++;
                                scene->removeItem(npcw3);
                                scene->removeItem(npcw2);
                                ui->dalej->show();
                            } else if (enemyHP<=0 && npcw2->health<0 && npcw3->health<0) {
                                scene->removeItem(niebieski_smok);
                                ui->dialogi->append("Pokonałeś Niebieskiego Smoka, ale nie udało ci się uratować mieszkańców. Dostajesz 100 XP.");
                                ChangeXP(100);
                                ChangeHP(200);

                                ui->zycie_wroga->hide();
                                deadBodies+=2;
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

/// scena, w której spotykamy ostatniego smoka
// jest to boss, który jest trudniejszy do pokonania
// ale dostajemy nową broń, która pomaga nam w walce
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

    if (touchedBridges >= 3) {

        ui->dialogi->append("Od tej pory możesz używać parasola, chroni cię on przed atakami.");
        main_character->setPos(10, 270);
        // ponownie ustawiamy fokus na postać
        main_character->setFlag(QGraphicsItem::ItemIsFocusable);
        main_character->setFocus();
        Character *wladca_zaru = spawnCharacter(":/images/images/wladca_zaru.png", 420, 150, 2500, 0, 0);
        wladca_zaru->hide();

        // bronie, którymi dysponuje smok
        QVector<Weapon*> bronieSmoka = {
            new Weapon("Ognisty oddech", 50),
            new Weapon("Zółty deszcz", 100),
            new Weapon("Mroczny pocisk", 150),
            new Weapon("Błyskawica", 150),
            new Weapon("Kamienne uderzenie", 300)
        };

        dialogue = true;
        if (dialogue == true) {
            FourthDialogue();
            dialogue = false;
        }
        else if (xp >= 400){
            ui->dialogi->append("Zyskujesz nową broń! Potężny HYDRANT!");
            PassEquipment(SpawnWeapon("HYDRANT POTĘŻNY", 70));
        }
        enemyHP = 0;
        ChangeEnemyHP(wladca_zaru->health);

        QTimer::singleShot(600, [=]() {
            // tym razem smok jest bardziej agresywny
            // ale nie rusza się
            wladca_zaru->show();
            ui->zycie_wroga->show();
        });
        QTimer::singleShot(800, [=]() {
            QTimer *atakWroga = new QTimer(this);
            connect(atakWroga, &QTimer::timeout, this, [=]() {
                weaponPower = chosenItem->getMoc();

                int randomIndex = rand() % bronieSmoka.size();
                Weapon *wybranaBron = bronieSmoka[randomIndex];
                ui->dialogi->append("Smok używa " + wybranaBron->getNazwa() + "!");
                // ui->dialogi->append(QString::number(randomIndex));

                QTimer *atakNasz = new QTimer(this);
                connect(atakNasz, &QTimer::timeout, this, [=]() {
                    if (main_character->getCzyAtak()) {
                        ChangeEnemyHP(-weaponPower);
                        main_character->setCzyAtak(false); // mamy 800 ms na atak
                    }
                });
                atakNasz->start(800);
                if (chosenItem->getNazwa() != "Parasol") {
                    main_character->setPixmap(QPixmap(":/images/images/postac.png"));
                    ui->dialogi->append("Smok zaatakował cię " + wybranaBron->getNazwa() + ". Tracisz " + QString::number(wybranaBron->getMoc()) + " punktów życia.");
                    ChangeHP(-wybranaBron->getMoc());
                    if (wybranaBron->getNazwa() == "Kamienne uderzenie") {
                        ui->dialogi->append("Dostałeś bronią ostateczną!"); // ostateczna broń
                    }
                } else if (chosenItem->getNazwa() == "Parasol") {
                    ui->dialogi->append("Używasz Parasola, jesteś chroniony."); // chroni przed atakami
                    main_character->setPixmap(QPixmap(":/images/images/postac_z_parasolem.png"));
                } else if (hp <= 0) {
                    TheEnd();
                } else if (wladca_zaru->health <= 0 && deadBodies < 3) {
                    wladca_zaru->hide();
                    scene->removeItem(wladca_zaru);
                    ui->zycie->hide();
                    ui->xp->hide();
                    ui->ekw_but->hide();
                    ui->dziennik->hide();
                    ui->dialogi->clear();
                    WinDialogue();
                    atakWroga->stop();
                } else if (wladca_zaru->health <= 0 || deadBodies >= 3) {
                    wladca_zaru->hide();
                    scene->removeItem(wladca_zaru);
                    ui->zycie->hide();
                    ui->xp->hide();
                    ui->ekw_but->hide();
                    ui->dziennik->hide();
                    ui->dialogi->clear();
                    LoseDialogue();
                    atakWroga->stop();

                }
            });
            atakWroga->start(2500);
        });

    } else {
        ui->zycie->hide();
        ui->xp->hide();
        ui->ekw_but->hide();
        ui->dziennik->hide();

        FinalDialogue(); // jeśli gracz nie zdobył wszystkich mostów
        connect(ui->dalej, &QPushButton::clicked, this, &MainWindow::TheEnd);
    }
}

// funkcja do przekazywania przedmiotów do ekwipunku
void MainWindow::PassEquipment(Weapon *przedmiot) {
    equi.push_back(przedmiot);
    QString nazwa = przedmiot->getNazwa();
    ui->dialogi->append("Zdobyłeś: " + nazwa.toUpper() + ". Zobacz swój ekwipunek");
}

// funkcja do wyświetlania ekwipunku
void MainWindow::ShowEquipment() {
    if(ui->ekw_list->isVisible()) {
        ui->ekw_list->hide();
    } else {
        ui->ekw_list->clear();

        if (!equi.empty()) {
            for(Weapon *przedmiot : equi) {
                ui->ekw_list->addItem(przedmiot->getNazwa());
            }
        } else {
            ui->ekw_list->addItem("PUSTO");
        }
        ui->ekw_list->show();
    }
    disconnect(ui->ekw_list, &QListWidget::itemClicked, nullptr, nullptr);

    connect(ui->ekw_list, &QListWidget::itemClicked, this, [=](QListWidgetItem *item){
        for(Weapon *przedmiot : equi) {
            if (przedmiot->getNazwa() == item->text()) {
                ui->dialogi->append("Wybrano przedmiot: " + przedmiot->getNazwa() + " o mocy: " + QString::number(przedmiot->getMoc()));
                // ui->dialogi->append("Moc: " + QString::number(przedmiot->getMoc()));

                chosenItem = przedmiot;
                main_character->setFlag(QGraphicsItem::ItemIsFocusable);
                main_character->setFocus();
                break;
            }
        }
    });
}

/// funkcja do wyświetlania dziennika
void MainWindow::ShowDiary() {
    if(ui->dzien_list->isVisible()) {
        ui->dzien_list->hide();
    } else {
        ui->dzien_list->clear();

        if (!diary.empty()) {
            for(const QString& item : diary) {
                ui->dzien_list->addItem(item);
            }
        } else {
            ui->dzien_list->addItem("PUSTO");
        }
        ui->dzien_list->show();
    }
}

// funkcja do dodawania postaci
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

// funkcja do dodawania broni
Weapon* MainWindow::SpawnWeapon(const QString& nazwa, int moc) {
    Weapon *bron = new Weapon(nazwa, moc);
    return bron;
}

// funkcja do sprawdzania, czy można atakować tzn. czy gracz jest daleko od smoka
bool attackPossibility(int xGracza, int yGracza, int xSmoka, int ySmoka) {
    return (qAbs(xGracza - xSmoka) >= 30 || qAbs(yGracza - ySmoka) >= 30);
}

// funkcja do poruszania się smoka w kierunku gracza
void MainWindow::MoveCharacter(Character *smok, Character *gracz) {
    qreal dx = gracz->x() - smok->x();
    qreal dy = gracz->y() - smok->y();
    qreal dlugosc = sqrt(dx*dx + dy*dy);
    dx /= dlugosc;
    dy /= dlugosc;

    qreal predkoscSmoka = smok->speed;

    smok->setPos(smok->x() + predkoscSmoka * dx, smok->y() + predkoscSmoka * dy);
}

// funkcja do zmiany tła
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

// funkcja do zmiany punktów życia
void MainWindow::ChangeHP(int punkty) {
    if (hp > 0) {
        hp += punkty;
        ui->zycie->setText("Życie: " + QString::number(hp));
    }
    else {
        TheEnd();
    }
}

// funkcja do zmiany punktów życia wroga
void MainWindow::ChangeEnemyHP(int punkty) {
    enemyHP += punkty;
    ui->zycie_wroga->setText("Życie wroga: " + QString::number(enemyHP));
}

// funkcja do zmiany punktów doświadczenia
void MainWindow::ChangeXP(int punkty) {
    xp += punkty;
    ui->xp->setText("XP: " + QString::number(xp));
}

// funkcja do wyświetlania końca gry
void MainWindow::TheEnd() {
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

// funkcje z dialogami
void MainWindow::FirstDialogue() {

    QStringList texts = { // wykorzystujemy QStringList, aby łatwiej było dodawać nowe dialogi
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
        QTimer::singleShot(100, this, &MainWindow::FirstDialogue);
        break;
    case 2:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::FirstDialogue);
        break;
    case 3:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::FirstDialogue);
        break;
    case 4:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::FirstDialogue);
        break;
    case 5:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::FirstDialogue);
        break;
    case 6:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::FirstDialogue);
        break;
    case 7:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(100, this, &MainWindow::FirstDialogue);
        break;
    case 8:
        ui->dialogi->append(texts[counter - 1]);
        QTimer::singleShot(1000, this, &MainWindow::FirstDialogue);
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

void MainWindow::SecondDialogue() {

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
            Weapon *parasol = nullptr;
            if (i == 6) {
                parasol = SpawnWeapon("Parasol", 0);
                PassEquipment(parasol);
                diary.push_back("Parasol będzie ci potrzebny podczas walki z bossem.");
            }
        });
    }
}

void MainWindow::ThirdDialogue() {

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
            Weapon *gasnica = nullptr;
            if (i == 4) {
                gasnica = SpawnWeapon("Gaśnica", 25);
                PassEquipment(gasnica);
            }
        });
    }
}

void MainWindow::FinalDialogue() {

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

void MainWindow::FourthDialogue() {
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

void MainWindow::WinDialogue() {
    QStringList texts = {
        "Brawo! Uratowałeś miasto i dzięki tobie wszyscy mogą być znowu szczęśliwi.",
        "Dodatkowo uratowałeś wszystkich mieszkańców, których napotkałeś na swojej drodze!",
        "Jesteś dobroduszny, potrafisz pracować zespołowo i nie boisz się wyzwań.",
        "Wszyscy w wiosce życzą ci wszystkiego najlepszego!",
        "Oby więcej takich udanych akcji, Młody Strażaku!"
    };

    for (int i = 0; i < texts.size(); ++i) {
        QTimer::singleShot(1000 * (i + 1), this, [=]() {
            counter++;
            ui->dialogi->append(texts[i]);
            if (i == 4) {
                TheEnd();
            }
        });
    }
}

void MainWindow::LoseDialogue() {
    QStringList texts = {
        "Brawo! Uratowałeś miasto i dzięki tobie wszyscy mogą być znowu szczęśliwi.",
        "Niestety, jednak nie udało ci się uratować wszystkich napotkanych na swojej drodze. Jest to niezwykle przykre.",
        "Będziesz musiał borykać się z wyrzutami sumienia aż do końca życia.",
        "Nie mniej jednak, ludzie są Ci wdzięczni, że ocaliłeś miasto.",
        "Tylko gdzie te dzieci..."
    };

    for (int i = 0; i < texts.size(); ++i) {
        QTimer::singleShot(1000 * (i + 1), this, [=]() {
            counter++;
            ui->dialogi->append(texts[i]);
            if (i == 4) {
                TheEnd();
            }
        });
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_exitGame_2_triggered()
{
    QApplication::quit();
}

