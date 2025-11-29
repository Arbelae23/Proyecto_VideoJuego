#include "Enemigos.h"
#include <QtMath>
#include <cstdlib>

Enemigos::Enemigos() {
    pos_f = QPointF(0,0);
    pos_base = QPointF(0,0);
    velocidad = QPointF(0,0);
    tamaño = QSize(40,40);
    bounds = QRect(int(pos_f.x()), int(pos_f.y()), tamaño.width(), tamaño.height());
}

void Enemigos::update(double dt, int width, int height) {
    if (!activo) return;

    // ---- Mover la base con velocidad ----
    pos_base += velocidad * dt;

    // ---- Rebote elástico perfecto ----
    if (pos_base.x() < 0) { pos_base.setX(0); velocidad.setX(fabs(velocidad.x())); }
    if (pos_base.x() + tamaño.width() > width) { pos_base.setX(width - tamaño.width()); velocidad.setX(-fabs(velocidad.x())); }
    if (pos_base.y() < 0) { pos_base.setY(0); velocidad.setY(fabs(velocidad.y())); }
    if (pos_base.y() + tamaño.height() > height) { pos_base.setY(height - tamaño.height()); velocidad.setY(-fabs(velocidad.y())); }

    // ---- Movimientos especiales ----
    switch (tipo_movimiento) {
    case TM_Linear:
        pos_f = pos_base;
        break;

    case TM_SenoDown:
        tiempo_sen += dt;
        pos_f.setX(pos_base.x() + amplitud_seno * qSin(tiempo_sen * frecuencia_seno));
        pos_f.setY(pos_base.y());
        break;

    case TM_Espiral:
        angulo += velAngular * dt;
        if (angulo > M_PI*2) angulo -= M_PI*2;
        radio_actual += expansion * dt;
        pos_f.setX(pos_base.x() + qCos(angulo) * radio_actual);
        pos_f.setY(pos_base.y() + qSin(angulo) * radio_actual);
        break;
    }

    // actualizar bounds
    bounds.moveTopLeft(QPoint(int(pos_f.x()), int(pos_f.y())));
}

void Enemigos::draw(QPainter &p) {
    if (!activo) return;
    p.setBrush(Qt::red);
    p.drawEllipse(bounds);
}
