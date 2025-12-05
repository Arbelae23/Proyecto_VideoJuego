#ifndef JUGADOR_R_L_H
#define JUGADOR_R_L_H

#include <QRect>
#include <QString>
#include <QPainter>

class Jugador_R_L {
public:
    Jugador_R_L();

    int vidas;
    QRect rect;
    QString skin;
    int carril;
    float speed;

    void moverDerecha();
    void moverIzquierda();
    void draw(QPainter &p);
    void quitarVida(int d);
    void moverPorVector(float dx, float dy);

    QRect getBounds() const { return rect; }
};

#endif
