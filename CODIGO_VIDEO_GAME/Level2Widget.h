#ifndef LEVEL2WIDGET_H
#define LEVEL2WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <vector>
#include "Enemigos.h"
#include "Jugador_R_L.h"
#include "Interacciones.h"
#include "Media.h" // asumo que tienes esta clase

class Level2Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Level2Widget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;

private:
    // miembros
    QTimer timer;
    double dt;
    double t_global;

    // Jugador / media / interacciones
    Jugador_R_L jugador;
    Media media;
    Interacciones inter;

    // background
    QPixmap background;

    // enemigos
    std::vector<Enemigos> enemigos;
    bool enemigosCreados = false;

    // controles
    bool wPressed = false;
    bool aPressed = false;
    bool sPressed = false;
    bool dPressed = false;

    // animación sprites jugador
    int currentFrame = 0;
    int desiredFrame = 0;
    int animAccumulatorMs = 0;
    int animStepMs = 80; // ms entre pasos
    const double animStepSeconds = 0.08;

    // funciones
    void setupEnemies();
    void moverJugadorWASD();
    void updatePlayerSkin();
    void checkCollisions();


    // GAMEOVER

    bool mostrarGameOver = false;
    QPixmap gameOverImg;

private slots:
    void onTick();
};

#endif // LEVEL2WIDGET_H
