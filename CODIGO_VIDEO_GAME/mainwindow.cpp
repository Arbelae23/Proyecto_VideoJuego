#include "MainWindow.h"
#include <QStackedWidget>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QResizeEvent>
#include <QStyle>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    stack(new QStackedWidget(this)),
    menuWidget(nullptr),
    backButton(nullptr),
    btnLevel1(nullptr),
    btnLevel2(nullptr),
    btnLevel3(nullptr)
{
    setWindowTitle("La Revolución Automotriz");
    setMinimumSize(800, 600);

    // preparo la UI: menú + 3 pantallas de nivel
    setupMenu();
    stack->addWidget(menuWidget);

    // crear 3 "pantallas" de nivel (placeholders)
    stack->addWidget(createLevelWidget(1)); // index 1
    stack->addWidget(createLevelWidget(2)); // index 2
    stack->addWidget(createLevelWidget(3)); // index 3

    setCentralWidget(stack);

    // Botón "Volver al menú" que aparece en los niveles
    backButton = new QPushButton("Volver al menú", this);
    backButton->setVisible(false);
    backButton->setFixedSize(120, 36);
    connect(backButton, &QPushButton::clicked, this, &MainWindow::onBackToMenu);

    // Posicionar el botón (se reposiciona en resizeEvent)
    int margin = 10;
    backButton->move(width() - backButton->width() - margin, margin);
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    // mantener el boton en la esquina superior derecha con un margen
    if (backButton) {
        int margin = 10;
        backButton->move(width() - backButton->width() - margin, margin);
    }
}

void MainWindow::setupMenu()
{
    menuWidget = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(menuWidget);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(30);
    mainLayout->setAlignment(Qt::AlignCenter);

    // Título
    QLabel *title = new QLabel("LA REVOLUCIÓN AUTOMOTRIZ", menuWidget);
    QFont f = title->font();
    f.setPointSize(24);
    f.setBold(true);
    title->setFont(f);
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    // Subtítulo / instrucción
    QLabel *subtitle = new QLabel("Selecciona un nivel para jugar", menuWidget);
    QFont f2 = subtitle->font();
    f2.setPointSize(12);
    subtitle->setFont(f2);
    subtitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(subtitle);

    // Botones de niveles
    btnLevel1 = new QPushButton("Nivel 1 — Carro antiguo (vista superior)", menuWidget);
    btnLevel1->setFixedSize(380, 48);
    btnLevel2 = new QPushButton("Nivel 2 — Carro lateral (vista superior lateral)", menuWidget);
    btnLevel2->setFixedSize(380, 48);
    btnLevel3 = new QPushButton("Nivel 3 — Moto (3ª persona, atrás)", menuWidget);
    btnLevel3->setFixedSize(380, 48);

    // Acomodar botones centrados en una caja
    QVBoxLayout *btnLayout = new QVBoxLayout;
    btnLayout->setSpacing(16);
    btnLayout->setAlignment(Qt::AlignCenter);
    btnLayout->addWidget(btnLevel1, 0, Qt::AlignHCenter);
    btnLayout->addWidget(btnLevel2, 0, Qt::AlignHCenter);
    btnLayout->addWidget(btnLevel3, 0, Qt::AlignHCenter);

    mainLayout->addLayout(btnLayout);

    // Conexiones
    connect(btnLevel1, &QPushButton::clicked, this, &MainWindow::onLevel1);
    connect(btnLevel2, &QPushButton::clicked, this, &MainWindow::onLevel2);
    connect(btnLevel3, &QPushButton::clicked, this, &MainWindow::onLevel3);
}

QWidget* MainWindow::createLevelWidget(int level)
{
    // Podrás reemplazar este placeholder por tu GameWidget específico por nivel
    QWidget *page = new QWidget(this);
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(20, 20, 20, 20);

    // Título del nivel
    QLabel *lblTitle = new QLabel(page);
    QFont ft = lblTitle->font();
    ft.setPointSize(18);
    ft.setBold(true);
    lblTitle->setFont(ft);
    lblTitle->setAlignment(Qt::AlignCenter);

    // Descripción / instrucciones del nivel
    QLabel *lblDesc = new QLabel(page);
    lblDesc->setWordWrap(true);
    lblDesc->setAlignment(Qt::AlignCenter);

    if (level == 1) {
        lblTitle->setText("Nivel 1 — Carro antiguo (vista superior)");
        lblDesc->setText("Conduce un carro antiguo visto desde arriba y esquiva obstáculos que aparecen. Usa las flechas izquierda/derecha para moverte.");
        page->setStyleSheet("background-color: #f7f7f7;");
    } else if (level == 2) {
        lblTitle->setText("Nivel 2 — Carro (vista lateral)");
        lblDesc->setText("Conduce de forma lateral y evita policías y bicicletas. Usa izquierda/derecha para esquivar.");
        page->setStyleSheet("background-color: #eef7ff;");
    } else {
        lblTitle->setText("Nivel 3 — Moto (3ª persona)");
        lblDesc->setText("Conduce una moto vista desde atrás, esquivando autos con movimientos sólo izquierda/derecha.");
        page->setStyleSheet("background-color: #fff5ee;");
    }

    // Espacio para un área donde luego integrarás tu GameWidget
    QLabel *gameArea = new QLabel(page);
    gameArea->setText("\n[ AREA DE JUEGO - Aquí irá tu GameWidget específico del nivel ]\n\n(En este demo es sólo un placeholder)\n");
    gameArea->setAlignment(Qt::AlignCenter);
    gameArea->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    gameArea->setMinimumHeight(300);
    gameArea->setStyleSheet("background: white;");

    // Agregar a layout
    lay->addWidget(lblTitle);
    lay->addSpacing(8);
    lay->addWidget(lblDesc);
    lay->addSpacing(12);
    lay->addWidget(gameArea, 1);

    return page;
}

// SLOTS
void MainWindow::onLevel1()
{
    // indice 1 en el stack -> nivel 1
    stack->setCurrentIndex(1);
    backButton->setVisible(true);
}

void MainWindow::onLevel2()
{
    stack->setCurrentIndex(2);
    backButton->setVisible(true);
}

void MainWindow::onLevel3()
{
    stack->setCurrentIndex(3);
    backButton->setVisible(true);
}

void MainWindow::onBackToMenu()
{
    stack->setCurrentIndex(0); // volver al menu principal
    backButton->setVisible(false);
}
