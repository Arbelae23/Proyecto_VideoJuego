#ifndef JUGADOR_U_D_DIAGONALS_H
#define JUGADOR_U_D_DIAGONALS_H

#include "Jugador_R_L.h"
#include <QPoint>

class Jugador_U_D_Diagonals : public Jugador_R_L {
public:
    Jugador_U_D_Diagonals();

    QPoint Pos_XY;

    // movimientos diagonales / WASD
    void moverW(); // up
    void moverS(); // down
    void moverA(); // left
    void moverD(); // right
    void moverWD(); // up-right
    void moverDS(); // down-right
    void moverAS(); // up-left
    void moverAW(); // down-left
};

#endif // JUGADOR_U_D_DIAGONALS_H
