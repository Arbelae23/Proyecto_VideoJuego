#ifndef JUGADOR_U_D_DIAGONALS_H
#define JUGADOR_U_D_DIAGONALS_H

#include "Jugador_R_L.h"
#include <cmath>

class Jugador_U_D_Diagonals : public Jugador_R_L {
public:
    Jugador_U_D_Diagonals();

    // Movimientos simples
    void moverW();
    void moverS();
    void moverA();
    void moverD();

    // Diagonales (corregidas)
    void moverWD();
    void moverDS();
    void moverAS();
    void moverAW();

    // Método recomendado (vector)
    void moverPorVector(float vx, float vy);
};

#endif
