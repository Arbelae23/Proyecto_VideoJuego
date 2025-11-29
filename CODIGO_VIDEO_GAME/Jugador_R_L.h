#ifndef JUGADOR_R_L_H
#define JUGADOR_R_L_H

#include <QRect>
#include <QPainter>

class Jugador_R_L {
public:
    Jugador_R_L();

    // atributos basicos
    int vidas;
    QRect rect;      // posicion y tamaño en pixeles del jugador
    QString skin;    // nombre o ruta de sprite (vacío = rect simple)
    int carril;      // opcional para niveles con carriles

    // movimiento
    int speed;

    // metodos
    void moverDerecha();
    void moverIzquierda();
    void draw(QPainter &p);
    QRect getBounds() const { return rect; }

    void quitarVida(int d = 1);
};

#endif // JUGADOR_R_L_H
