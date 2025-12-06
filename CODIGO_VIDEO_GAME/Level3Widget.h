#ifndef LEVEL3WIDGET_H
#define LEVEL3WIDGET_H

#include <QWidget>
#include <QTimer>
#include "Jugador_R_L.h"
#include "Enemigos.h"
#include "Interacciones.h"
#include "Media.h"
#include <vector>
#include <QPixmap>

class Level3Widget : public QWidget {
    Q_OBJECT
public:
    explicit Level3Widget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onTick();

private:
    QTimer timer;
    double dt;

    Jugador_R_L jugador; // representará la moto desde atrás
    Interacciones inter;
    Media media;
    QPixmap background; // imagen de fondo del nivel 3

    // Carretera con valores fijos (px) para fácil modificación
    int roadX;      // posición X donde empieza la carretera
    int roadWidth;  // ancho de la carretera

    // Flag para inicializar la posición vertical del jugador cuando el widget ya tiene tamaño
    bool playerPosInitialized = false;

    // Estado de animación del jugador (−2..0..+2)
    int currentFrame = 0;
    int desiredFrame = 0;
    void updatePlayerSkin();

    // Estado de teclas mantenidas para movimiento continuo
    bool leftHeld = false;
    bool rightHeld = false;

    // Temporización de transición de animación
    int animAccumulatorMs = 0;   // acumula tiempo hasta el próximo paso
    int animStepMs = 100;        // duración actual del paso
    int animMinMs = 80;          // mínimo del rango deseado
    int animMaxMs = 120;         // máximo del rango deseado

    std::vector<Enemigos> enemigos;
    void spawnObstacles();
    void checkCollisions();

    // Periodo de gracia al inicio: enemigos visibles pero sin daño
    bool graceActive = false;
    int graceMsRemaining = 3000; // milisegundos de gracia
    bool firstSpawnDone = false; // reservado (no usado)
    int skipSpawnsRemaining = 0; // sin saltos de oleadas

    // Ajustes de colisión específicos de Nivel 3
    bool collisionEnabledL3 = true;          // permite deshabilitar colisiones en L3 si se desea
    double l3ExtraInsetXRatio = 0.15;        // reducción adicional horizontal del hitbox del jugador (por lado)
    double l3ExtraInsetYRatio = 0.08;        // reducción adicional vertical del hitbox del jugador (por lado)
    double l3MinOverlapAreaRatio = 0.08;     // área mínima de solapamiento relativa al área del enemigo

    // Lineas fijas para spawn de enemigos (X) y altura fija (Y)
    int laneX1 = 580;  // carril izquierdo (px)
    int laneX2 = 625;  // carril centro (px)
    int laneX3 = 680; // carril derecho (px)
    int laneSpawnY = 400; // altura de aparición (negativo = arriba, fuera de pantalla)
};

#endif // LEVEL3WIDGET_H
