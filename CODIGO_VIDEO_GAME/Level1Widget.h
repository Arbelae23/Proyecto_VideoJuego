#ifndef LEVEL1WIDGET_H
#define LEVEL1WIDGET_H

#include <QWidget>
#include <QTimer>
#include "Jugador_R_L.h"
#include "Enemigos.h"
#include "Interacciones.h"
#include "Media.h"
#include <vector>
#include <QPixmap>

class Level1Widget : public QWidget {
    Q_OBJECT
public:
    explicit Level1Widget(QWidget *parent = nullptr);
    void reiniciarNivel1();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onTick();

private:
    QTimer timer;
    double dt;

    Jugador_R_L jugador;
    Interacciones inter;
    Media media;
    QPixmap background;  // imagen de fondo del nivel


    std::vector<Enemigos> enemigos; // obstáculos móviles simples
    void spawnEnemies();
    void checkCollisions();

    // Coordenadas de dibujo del camino (solo visual)
    int roadX = 0;
    int roadWidth = 0;

    // Límites fijos de movimiento (independientes del camino)
    int moveXMin = 0;
    int moveXMax = 0;

    // Aparición del jugador: margen inferior ajustable (px, usar valores POSITIVOS)
    // Menor valor = más abajo; mayor valor = más arriba.
    int playerBottomMargin = 65; // cambia este valor para ajustar la altura
    bool playerPosInitialized = false;

    // Flags de teclas mantenidas
    bool leftHeld = false;
    bool rightHeld = false;

    // Velocidad de movimiento del jugador en Nivel 1 (px por pulsación)
    int playerMoveSpeed = 16; // sube este valor para más velocidad

    // =========================
    // GAME OVER / VICTORIA
    // =========================
    bool mostrarGameOver = false;
    bool mostrarVictoria = false;
    bool nivelGanado = false;
    bool esperandoDecision = false;
    QPixmap gameOverImg;
    QPixmap victoriaImg;

    // =========================
    // TIEMPO DEL NIVEL (igual que Nivel 2)
    // =========================
    double tiempoLimite = 30.0;
    double tiempoRestante = 30.0;

signals:
    void volverAlMenu();
};

#endif // LEVEL1WIDGET_H
