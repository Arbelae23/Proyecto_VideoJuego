#include "Jugador_R_L.h"

Jugador_R_L::Jugador_R_L()
    : vidas(3), rect(100, 100, 50, 80), skin(""), carril(0), speed(8)
{}

void Jugador_R_L::moverDerecha() {
    rect.translate(speed, 0);
}

void Jugador_R_L::moverIzquierda() {
    rect.translate(-speed, 0);
}

void Jugador_R_L::draw(QPainter &p) {
    if (!skin.isEmpty()) {
        QPixmap sprite;
        if (sprite.load(skin)) {
            p.drawPixmap(rect, sprite);
        } else {
            // Si falla la imagen, dibuja caja roja
            p.setBrush(Qt::red);
            p.drawRect(rect);
        }
    } else {
        p.setBrush(Qt::blue);
        p.drawRect(rect);
    }
}


void Jugador_R_L::quitarVida(int d) {
    vidas -= d;
    if (vidas < 0) vidas = 0;
}

void Jugador_R_L::moverPorVector(float dx, float dy)
{
    rect.translate(int(dx), int(dy));
}
