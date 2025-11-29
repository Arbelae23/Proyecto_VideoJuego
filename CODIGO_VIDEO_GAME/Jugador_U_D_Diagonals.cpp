#include "Jugador_U_D_Diagonals.h"

Jugador_U_D_Diagonals::Jugador_U_D_Diagonals()
    : Jugador_R_L(), Pos_XY(0,0)
{}

void Jugador_U_D_Diagonals::moverW()  { rect.translate(0, -speed); }
void Jugador_U_D_Diagonals::moverS()  { rect.translate(0, speed); }
void Jugador_U_D_Diagonals::moverA()  { rect.translate(-speed, 0); }
void Jugador_U_D_Diagonals::moverD()  { rect.translate(speed, 0); }
void Jugador_U_D_Diagonals::moverWD() { rect.translate(speed, -speed); }
void Jugador_U_D_Diagonals::moverDS() { rect.translate(speed, speed); }
void Jugador_U_D_Diagonals::moverAS() { rect.translate(-speed, -speed); }
void Jugador_U_D_Diagonals::moverAW() { rect.translate(-speed, speed); }
