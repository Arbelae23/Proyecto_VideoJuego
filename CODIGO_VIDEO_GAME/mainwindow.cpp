#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Media.h"
#include <QDebug>
#include <QPainter>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const int W = 1330;
    const int H = 700;

    // Fondo del menú desde Media (dibujado manualmente para evitar duplicación)
    try {
        Media media;
        const QPixmap &menuBg = media.getBackgroundMenu();
        if (!menuBg.isNull()) {
            menuBgOrig = menuBg;
            ui->pageMenu->installEventFilter(this);
            ui->pageMenu->setAttribute(Qt::WA_OpaquePaintEvent);
        }
    } catch (const MediaLoadError &ex) {
        qDebug() << "Menu background load failed:" << ex.what();
    }

    // Crear niveles
    level1 = new Level1Widget();
    level2 = new Level2Widget();
    level3 = new Level3Widget();

    // Insertar cada nivel en su página correcta
    level1->setParent(ui->pageLevel1);
    level1->setGeometry(10, 0, W, H);

    level2->setParent(ui->pageLevel2);
    level2->setGeometry(10, 0, W, H);   // ✅ AHORA IGUAL QUE LOS OTROS

    level3->setParent(ui->pageLevel3);
    level3->setGeometry(10, 0, W, H);

    // Conectar botones
    connect(ui->btnLevel1, &QPushButton::clicked, this, &MainWindow::openLevel1);
    connect(ui->btnLevel2, &QPushButton::clicked, this, &MainWindow::openLevel2);
    connect(ui->btnLevel3, &QPushButton::clicked, this, &MainWindow::openLevel3);
    connect(ui->btnBack, &QPushButton::clicked, this, &MainWindow::returnToMenu);
    connect(level2, &Level2Widget::volverAlMenu,
            this, &MainWindow::returnToMenu);
    connect(level1, &Level1Widget::volverAlMenu,
            this, &MainWindow::returnToMenu);
    connect(level3, &Level3Widget::volverAlMenu,
            this, &MainWindow::returnToMenu);
    connect(level2, &Level2Widget::volverAlMenu,
            this, &MainWindow::returnToMenu);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (ui && ui->pageMenu) {
        ui->pageMenu->update();
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->pageMenu && event->type() == QEvent::Paint) {
        QPainter p(ui->pageMenu);
        if (!menuBgOrig.isNull()) {
            // Escalar a tamaño fijo solicitado y dibujar en la esquina superior izquierda
            QPixmap scaled = menuBgOrig.scaled(QSize(menuBgTargetW, menuBgTargetH), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            p.drawPixmap(0, 0, scaled);
            // Rellenar horizontalmente el espacio sobrante a la derecha para quitar el vacío
            int remaining = ui->pageMenu->width() - menuBgTargetW;
            if (remaining > 0) {
                QPixmap filler = menuBgOrig.scaled(QSize(remaining, menuBgTargetH), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                p.drawPixmap(menuBgTargetW, 0, filler);
            }
        }
        return true; // ya pintado
    }
    return QMainWindow::eventFilter(obj, event);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openLevel1() {
    level1->reiniciarNivel1();
    ui->stackedWidget->setCurrentIndex(1);
    level1->setFocus();
}

void MainWindow::openLevel2() {
    level2->reiniciarNivel2();   // 🔥 REINICIA TODO
    ui->stackedWidget->setCurrentIndex(2);
    level2->setFocus();
}


void MainWindow::openLevel3() {
    level3->reiniciarNivel3();
    ui->stackedWidget->setCurrentIndex(3);
    level3->setFocus();
}

void MainWindow::returnToMenu() {
    ui->stackedWidget->setCurrentIndex(0);
}
