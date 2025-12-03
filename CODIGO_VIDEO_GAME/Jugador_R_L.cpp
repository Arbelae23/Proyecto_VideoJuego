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
        // Aquí dibujarías un QPixmap si lo deseas
    } else {
        p.setBrush(Qt::blue);
        p.drawRect(rect);
    }
}

void Jugador_R_L::quitarVida(int d) {
    vidas -= d;
    if (vidas < 0) vidas = 0;
}
