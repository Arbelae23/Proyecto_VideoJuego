#ifndef ENEMIGOS_H
#define ENEMIGOS_H

#include <QPointF>
#include <QSize>
#include <QRect>
#include <QPainter>
#include <QString>
#include <QPixmap>

class Enemigos {
public:
    Enemigos();

    enum TipoMovimiento {
        TM_Linear,
        TM_SenoDown,
        TM_Espiral,
        TM_EspiralHorizontal
    };

    // Variables básicas
    bool activo = true;
    QString tipo_objeto = "enemigo";

    QPointF pos_f;       // posición final (para dibujo)
    QPointF pos_base;    // posición base (para rebotes)
    QPointF velocidad;   // velocidad real
    QSize tamaño;
    QRect bounds;
    TipoMovimiento tipo_movimiento = TM_Linear;


    // colisiones
    bool enChoque = false;
    double tiempoChoque = 0.0;
    void activarChoque();
    void desactivarChoque();

    // Movimiento sinusoidal
    double tiempo_sen = 0.0;
    double amplitud_seno = 50.0;
    double frecuencia_seno = 2.0;

    // Movimiento espiral
    double angulo = 0.0;
    double velAngular = 2.0;
    double radio_actual = 20.0;
    double expansion = 20.0;

    //Sprite enemigos
    QPointF pos_inicial;
    bool usaSprite = false;
    QPixmap sprite;
    QPixmap spriteNormal;
    QPixmap spriteChoque;

    // Rotación opcional del sprite (para "rodador")
    bool rotateSprite = false;
    double rotationAngle = 0.0;         // grados
    int rotationDir = 1;                // 1 horario, -1 antihorario
    double rotationSpeedDeg = 180.0;    // grados por segundo
    double rotationToggleAccum = 0.0;   // acumulador para alternar
    double rotationToggleInterval = 0.5;// segundos para alternar sentido

    // Atributo opcional de carril para Nivel 3
    int lane = -1; // 0..2 cuando se usa carril fijo



    // Funciones
    void update(double dt, int width, int height);
    void draw(QPainter &p);
    void stepRotation(double dt);

    // Mantener compatibilidad con tu código antiguo
    QRect getBounds() const { return bounds; }
};

#endif
