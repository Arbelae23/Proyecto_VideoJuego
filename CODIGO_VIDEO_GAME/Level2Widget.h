#ifndef LEVEL2WIDGET_H
#define LEVEL2WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPixmap>
#include <vector>

#include "Enemigos.h"
#include "Jugador_R_L.h"
#include "Interacciones.h"
#include "Media.h"
#include "Objeto_Interactivo.h"

class Level2Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Level2Widget(QWidget *parent = nullptr);
    void reiniciarNivel2();   // ✅ debe ser PUBLICO

protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;

private:
    // =========================
    // TIMER PRINCIPAL
    // =========================
    QTimer timer;
    double dt = 0.016;        // 60 FPS estable
    double t_global = 0.0;

    // =========================
    // JUGADOR / MEDIA / VIDA
    // =========================
    Jugador_R_L jugador;
    Media media;
    Interacciones inter;

    // =========================
    // FONDO
    // =========================
    QPixmap background;

    // =========================
    // ENEMIGOS
    // =========================
    std::vector<Enemigos> enemigos;
    bool enemigosCreados = false;

    void setupEnemies();

    // =========================
    // CONTROLES
    // =========================
    bool wPressed = false;
    bool aPressed = false;
    bool sPressed = false;
    bool dPressed = false;

    void moverJugadorWASD();
    void updatePlayerSkin();
    void checkCollisions();

    // =========================
    // ANIMACIÓN
    // =========================
    int currentFrame = 0;
    int desiredFrame = 0;
    int animAccumulatorMs = 0;
    int animStepMs = 80;

    // =========================
    // GAME OVER
    // =========================
    bool mostrarGameOver = false;
    bool esperandoDecision = false;
    QPixmap gameOverImg;

    // =========================
    // TROFEOS ✅ (ARREGLADO)
    // =========================
    std::vector<Objeto_Interactivo> trofeos;

    QTimer *timerTrofeos = nullptr;   // ✅ SIEMPRE inicializado
    double tiempoTrofeoVida = 5.0;    // dura 5 segundos
    int totalTrofeosObjetivo = 5;     // los que debes recoger para ganar
    int trofeosRecolectados = 0;
    bool nivelGanado = false;

    void spawnTrofeo();               // ✅ GENERADOR

    // =========================
    // TIEMPO DEL NIVEL
    // =========================
    double tiempoLimite = 30.0;
    double tiempoRestante = 30.0;

private slots:
    void onTick();

signals:
    void volverAlMenu();
};

#endif // LEVEL2WIDGET_H
