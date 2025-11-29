#ifndef ENEMIGOS_H
#define ENEMIGOS_H

#include <QPointF>
#include <QSize>
#include <QRect>
#include <QPainter>
#include <QString>

class Enemigos {
public:
    Enemigos();

    enum TipoMovimiento {
        TM_Linear,
        TM_SenoDown,
        TM_Espiral
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

    // Movimiento sinusoidal
    double tiempo_sen = 0.0;
    double amplitud_seno = 50.0;
    double frecuencia_seno = 2.0;

    // Movimiento espiral
    double angulo = 0.0;
    double velAngular = 2.0;
    double radio_actual = 20.0;
    double expansion = 20.0;

    // Funciones
    void update(double dt, int width, int height);
    void draw(QPainter &p);

    // Mantener compatibilidad con tu código antiguo
    QRect getBounds() const { return bounds; }
};

#endif
