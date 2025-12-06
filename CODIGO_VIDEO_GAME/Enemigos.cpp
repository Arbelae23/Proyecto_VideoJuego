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

        // hay que tratar de mantenerlo quieto solo durante la animación de choque
        pos_f += QPointF(0, 0);

        if (tiempoChoque >= 0.4)
        {
            enChoque = false;
            tiempoChoque = 0.0;

            QPointF dir = pos_f - pos_inicial;
            double mag = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
            if (mag != 0) dir /= mag;

            velocidad = velocidadOriginal;

            tiempoCooldown = cooldownGolpe;
            yaGolpeo = true;

            // Restaurar sprite normal
            if (!spriteNormal.isNull())
                sprite = spriteNormal;

            if (tipo_movimiento == TM_EspiralHorizontal)
            {
                // Bicicleta: empuje fuerte, pero no reiniciamos animación
                pos_f += dir * 180;      // empuje visual
                pos_base += dir * 180;   // mueve la base real
            }
            else if (tipo_movimiento == TM_Inteligente)
            {
                // IA: empuje normal, y actualizar base y pos_f
                pos_f += dir * 120;
                pos_base = pos_f;
                // no reiniciar tiempo_sen ni angulo
            }
            else
            {
                // Policías y otros enemigos
                pos_f += dir * 120;
                pos_base = pos_f;
                tiempo_sen = 0;
                angulo = 0;
                radio_actual = 0;
            }
        }

        bounds.moveTopLeft(QPoint(int(pos_f.x()), int(pos_f.y())));
        return;
    }


    if (tiempoCooldown > 0.0)
    {
        tiempoCooldown -= dt;
        yaGolpeo = true;   // aún no puede volver a dañar
    }
    else
    {
        yaGolpeo = false; // ya puede volver a dañar
    }



    if (!activo) return;

    // ---- Mover la base con velocidad ----
    pos_base += velocidad * dt;

    // ---- Rebote elástico perfecto ----
    if (pos_base.x() < 0) {
        pos_base.setX(0);
        velocidad.setX(fabs(velocidad.x()));

        // MIRAR A LA DERECHA
        mirandoDerecha = true;

        if (!spriteNormal.isNull())
            sprite = spriteNormal;
    }

    if (pos_base.x() + tamaño.width() > width) {
        pos_base.setX(width - tamaño.width());
        velocidad.setX(-fabs(velocidad.x()));
        mirandoDerecha = false;

        if (tipo_movimiento == TM_Inteligente) {
            // IA: siempre spriteNormal
            if (!spriteNormal.isNull()) sprite = spriteNormal;
        } else {
            // Otros enemigos: espejo
            if (!spriteNormalIzquierda.isNull()) sprite = spriteNormalIzquierda;
        }
    }

    // REBOTE SUPERIOR → BAJA
    if (pos_base.y() < 0)
    {
        pos_base.setY(0);
        velocidad.setY(fabs(velocidad.y()));   // ahora baja

        sprite = spriteNormalAbajo;            //  mirar abajo
        mirandoArriba = false;
    }

    // REBOTE INFERIOR → SUBE
    if (pos_base.y() + tamaño.height() > height)
    {
        pos_base.setY(height - tamaño.height());
        velocidad.setY(-fabs(velocidad.y()));  // ahora sube

        sprite = spriteNormalArriba;           //  mirar arriba
        mirandoArriba = true;
    }


    if (tipo_movimiento == TM_Inteligente && activo)
    {
        QPointF jugadorPos = objetivo;

        // vector hacia el jugador
        QPointF dir = jugadorPos - pos_f;
        double distancia = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());

        // DETECTAR JUGADOR
        if (distancia < radioVision)
            persiguiendo = true;
        else if (distancia > radioPerdida)
            persiguiendo = false;

        // MOVIMIENTO
        if (persiguiendo)
        {
            dir /= distancia; // normalizar
            pos_f += dir * velocidad.x() * dt;
        }
        else
        {
            // patrulla cuando no ve al jugador
            pos_f += velocidad * dt;

            if (pos_f.x() < 50 || pos_f.x() > width - 100)
                velocidad.setX(-velocidad.x());
        }

        bounds.moveTo(pos_f.toPoint());
    }


    // ---- Movimientos especiales ----
    switch (tipo_movimiento) {
    case TM_Linear:
        pos_f = pos_base;
        break;

    case TM_SenoDown:
        tiempo_sen += dt;
        pos_f.setX(pos_base.x() + amplitud_seno * qSin(tiempo_sen * frecuencia_seno));
        pos_f.setY(pos_base.y());

        // Selección dinámica de sprite según signos de dx/dy
        {
            const double dx_dt = amplitud_seno * frecuencia_seno * qCos(tiempo_sen * frecuencia_seno);
            const double dy_dt = velocidad.y();
            const double eps = 1e-3; // umbral pequeño para evitar parpadeos

            // Actualizar banderas de mirada
            if (dx_dt > eps) mirandoDerecha = true;
            else if (dx_dt < -eps) mirandoDerecha = false;
            if (dy_dt > eps) mirandoArriba = false;
            else if (dy_dt < -eps) mirandoArriba = true;

            // Elegir sprite según cuadrante/dirección
            if (dy_dt > eps && dx_dt > eps) {
                if (!spriteDiagonalSD.isNull()) sprite = spriteDiagonalSD; // abajo-derecha
                else if (!spriteNormalDerecha.isNull()) sprite = spriteNormalDerecha;
                else if (!spriteNormalAbajo.isNull()) sprite = spriteNormalAbajo;
            } else if (dy_dt > eps && dx_dt < -eps) {
                if (!spriteDiagonalSA.isNull()) sprite = spriteDiagonalSA; // abajo-izquierda
                else if (!spriteNormalIzquierda.isNull()) sprite = spriteNormalIzquierda;
                else if (!spriteNormalAbajo.isNull()) sprite = spriteNormalAbajo;
            } else if (dy_dt < -eps && dx_dt > eps) {
                if (!spriteDiagonalWD.isNull()) sprite = spriteDiagonalWD; // arriba-derecha
                else if (!spriteNormalDerecha.isNull()) sprite = spriteNormalDerecha;
                else if (!spriteNormalArriba.isNull()) sprite = spriteNormalArriba;
            } else if (dy_dt < -eps && dx_dt < -eps) {
                if (!spriteDiagonalWA.isNull()) sprite = spriteDiagonalWA; // arriba-izquierda
                else if (!spriteNormalIzquierda.isNull()) sprite = spriteNormalIzquierda;
                else if (!spriteNormalArriba.isNull()) sprite = spriteNormalArriba;
            } else if (dy_dt > eps) {
                if (!spriteNormalAbajo.isNull()) sprite = spriteNormalAbajo; // puro abajo
            } else if (dy_dt < -eps) {
                if (!spriteNormalArriba.isNull()) sprite = spriteNormalArriba; // puro arriba
            } else if (dx_dt > eps) {
                if (!spriteNormalDerecha.isNull()) sprite = spriteNormalDerecha; // puro derecha
                else if (!spriteNormal.isNull()) sprite = spriteNormal;
            } else if (dx_dt < -eps) {
                if (!spriteNormalIzquierda.isNull()) sprite = spriteNormalIzquierda; // puro izquierda
            }
        }
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

        //  SOLO SE DETIENE MOMENTÁNEAMENTE
        velocidad *= 0.0;
    }
}


void Enemigos::desactivarChoque()
{
    // Volver al sprite correcto según dirección
    if (mirandoDerecha) {
        // Fallback a spriteNormal si no hay variante explícita de derecha
        sprite = spriteNormalDerecha.isNull() ? spriteNormal : spriteNormalDerecha;
    } else {
        sprite = spriteNormalIzquierda;
    }

    activo = false;   // ahora sí se elimina
    enChoque = false;
}
