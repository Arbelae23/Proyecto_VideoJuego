#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Crear niveles
    level1 = new Level1Widget();
    level2 = new Level2Widget();
    level3 = new Level3Widget();

    // Insertar cada nivel en su página correcta
    level1->setParent(ui->pageLevel1);
    //level1->setGeometry(0, 0, ui->pageLevel1->width(), ui->pageLevel1->height());
    level1->setGeometry(10, 0, 1330, 700);

    level2->setParent(ui->pageLevel2);
    level2->setGeometry(0, 0, ui->pageLevel2->width(), ui->pageLevel2->height());

    level3->setParent(ui->pageLevel3);
    //level3->setGeometry(0, 0, ui->pageLevel3->width(), ui->pageLevel3->height());
    level3->setGeometry(10, 0, 1330, 700);


    // Conectar botones
    connect(ui->btnLevel1, &QPushButton::clicked, this, &MainWindow::openLevel1);
    connect(ui->btnLevel2, &QPushButton::clicked, this, &MainWindow::openLevel2);
    connect(ui->btnLevel3, &QPushButton::clicked, this, &MainWindow::openLevel3);
    connect(ui->btnBack, &QPushButton::clicked, this, &MainWindow::returnToMenu);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openLevel1() {
    ui->stackedWidget->setCurrentIndex(1);
    level1->setFocus();
}

void MainWindow::openLevel2() {
    ui->stackedWidget->setCurrentIndex(2);
    level2->setFocus();
}

void MainWindow::openLevel3() {
    ui->stackedWidget->setCurrentIndex(3);
    level3->setFocus();
}

void MainWindow::returnToMenu() {
    ui->stackedWidget->setCurrentIndex(0);
}

