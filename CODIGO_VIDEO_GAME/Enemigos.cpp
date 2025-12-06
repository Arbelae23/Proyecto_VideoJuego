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
            // terminar choque
            enChoque = false;
            tiempoChoque = 0.0;

            // Restaurar sprite según lo que exista, en prioridad:
            // 1) vertical (arriba/abajo) si están cargados
            // 2) horizontal (derecha/izquierda) si están cargados
            // 3) spriteNormal simple
            // 4) dejar spriteChoque si nada más (defensa)
            if (!spriteNormalArriba.isNull() || !spriteNormalAbajo.isNull())
            {
                // si sabemos si estaba mirando arriba/abajo (mirandoArriba)
                if (mirandoArriba && !spriteNormalArriba.isNull())
                    sprite = spriteNormalArriba;
                else if (!mirandoArriba && !spriteNormalAbajo.isNull())
                    sprite = spriteNormalAbajo;
                else if (!spriteNormalArriba.isNull())
                    sprite = spriteNormalArriba;
                else
                    sprite = spriteNormalAbajo;
            }
            else if (!spriteNormalDerecha.isNull() || !spriteNormalIzquierda.isNull())
            {
                if (mirandoDerecha && !spriteNormalDerecha.isNull())
                    sprite = spriteNormalDerecha;
                else if (!mirandoDerecha && !spriteNormalIzquierda.isNull())
                    sprite = spriteNormalIzquierda;
                else if (!spriteNormalDerecha.isNull())
                    sprite = spriteNormalDerecha;
                else
                    sprite = spriteNormalIzquierda;
            }
            else if (!spriteNormal.isNull())
            {
                sprite = spriteNormal;
            }
            else
            {
                // respaldo: si todo está vacío, no cambiar (mantener spriteChoque) o asignar spriteChoque para evitar círculo
                // aquí dejamos sprite = spriteChoque para evitar fallback vacío
                sprite = spriteChoque;
            }

            // opcional: resetear posición si quieres (tu código hacía pos_inicial)
            pos_base = pos_inicial;
            pos_f = pos_inicial;
            bounds.moveTopLeft(QPoint(int(pos_f.x()), int(pos_f.y())));
        }

        // Mientras estaba en choque no procesamos movimiento normal
        return;
    }


    if (!activo) return;

    // ---- Mover la base con velocidad ----
    pos_base += velocidad * dt;

    // ---- Rebote elástico perfecto ----
    if (pos_base.x() < 0) {
        pos_base.setX(0);
        velocidad.setX(fabs(velocidad.x()));

        // ✅ MIRAR A LA DERECHA
        mirandoDerecha = true;

        if (!spriteNormal.isNull())
            sprite = spriteNormal;
    }

    if (pos_base.x() + tamaño.width() > width) {
        pos_base.setX(width - tamaño.width());
        velocidad.setX(-fabs(velocidad.x()));

        // ✅ MIRAR A LA IZQUIERDA (ESPEJO)
        mirandoDerecha = false;
        sprite = spriteNormalIzquierda;
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
}

void Enemigos::draw(QPainter &p)
{
    if (!activo) return;

    if (usaSprite && !sprite.isNull()) {
        //  DIBUJAR SPRITE ESCALADO AL TAMAÑO DEL ENEMIGO
        p.drawPixmap(bounds, sprite);
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
    // 🔥 Volver al sprite correcto según dirección
    sprite = mirandoDerecha ? spriteNormalDerecha : spriteNormalIzquierda;

    activo = false;   // ahora sí se elimina
    enChoque = false;
}
