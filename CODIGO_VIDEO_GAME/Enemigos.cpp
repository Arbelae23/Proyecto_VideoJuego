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


    if (enChoque)
    {
        tiempoChoque += dt;

        if (tiempoChoque >= 0.4)
        {
            sprite = spriteNormal;
            pos_base = pos_inicial;
            pos_f = pos_inicial;
            bounds.moveTopLeft(QPoint(int(pos_f.x()), int(pos_f.y())));
            enChoque = false;
        }
        return;
    }

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

    case TM_EspiralHorizontal:
        tiempo_sen += dt;
        angulo += velAngular * dt;

        pos_f.setX(pos_base.x() + velocidad.x() * dt);
        pos_f.setY(pos_base.y() + qSin(angulo) * radio_actual);

        break;
    }

    // actualizar bounds
    bounds.moveTopLeft(QPoint(int(pos_f.x()), int(pos_f.y())));

    // Actualizar rotación si está habilitada
    stepRotation(dt);
}

void Enemigos::stepRotation(double dt)
{
    if (!rotateSprite) return;
    rotationToggleAccum += dt;
    if (rotationToggleAccum >= rotationToggleInterval) {
        rotationDir = -rotationDir; // alternar sentido
        rotationToggleAccum = 0.0;
    }
    rotationAngle += rotationDir * rotationSpeedDeg * dt;
    // mantener en [0, 360)
    if (rotationAngle >= 360.0) rotationAngle -= 360.0;
    if (rotationAngle < 0.0) rotationAngle += 360.0;
}

void Enemigos::draw(QPainter &p)
{
    if (!activo) return;

    if (usaSprite && !sprite.isNull()) {
        if (rotateSprite) {
            // Dibujar con rotación alrededor del centro del enemigo
            p.save();
            QPoint center = bounds.center();
            p.translate(center);
            p.rotate(rotationAngle);
            QPixmap scaled = sprite.scaled(bounds.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            QRect localRect(-bounds.width()/2, -bounds.height()/2, bounds.width(), bounds.height());
            p.drawPixmap(localRect.topLeft(), scaled);
            p.restore();
        } else {
            // Dibujar sprite escalado explícitamente al tamaño del enemigo
            QPixmap scaled = sprite.scaled(bounds.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            p.drawPixmap(bounds.topLeft(), scaled);
        }
    } else {
        // fallback: círculo rojo
        p.setBrush(Qt::red);
        p.drawEllipse(bounds);
    }
}



void Enemigos::activarChoque()
{
    if (!enChoque && !spriteChoque.isNull())
    {
        sprite = spriteChoque;
        enChoque = true;
        tiempoChoque = 0.0;
        velocidad = QPointF(0,0); // se detiene al chocar
    }
}

void Enemigos::desactivarChoque()
{
    sprite = spriteNormal;
    activo = false;   // ahora sí se elimina
    enChoque = false;
}
