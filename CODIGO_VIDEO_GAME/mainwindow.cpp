#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const int W = 1330;
    const int H = 700;

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
