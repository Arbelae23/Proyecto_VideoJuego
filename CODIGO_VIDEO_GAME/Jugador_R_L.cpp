#include "Jugador_R_L.h"
#include <algorithm>

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

QRect Jugador_R_L::getHitbox() const
{
    // Reducir el área efectiva de colisión para evitar choques con bordes transparentes.
    // Se usa un porcentaje del tamaño actual para que escale con el sprite.
    const double insetXRatio = 0.20; // 20% por lado
    const double insetYRatio = 0.10; // 10% por lado

    int insetX = int(rect.width() * insetXRatio);
    int insetY = int(rect.height() * insetYRatio);

    // Asegurar que el hitbox no colapse en tamaños muy pequeños
    int minHalfW = 2;
    int minHalfH = 2;
    insetX = std::min(insetX, std::max(0, rect.width()/2 - minHalfW));
    insetY = std::min(insetY, std::max(0, rect.height()/2 - minHalfH));

    return rect.adjusted(insetX, insetY, -insetX, -insetY);
}
