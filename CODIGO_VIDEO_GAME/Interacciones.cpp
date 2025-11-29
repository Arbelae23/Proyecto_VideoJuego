#include "Interacciones.h"
#include <QDebug>

Interacciones::Interacciones(QObject *parent)
    : QObject(parent), trofeos(0), contador_vidas(3), cuenta_atras(0), game_over(false)
{}

void Interacciones::sumar_Trofeos(int n) {
    trofeos += n;
    emit trofeosChanged(trofeos);
}

void Interacciones::quitar_vida(int n) {
    contador_vidas -= n;
    if (contador_vidas < 0) contador_vidas = 0;
    emit vidasChanged(contador_vidas);
    if (contador_vidas == 0) gameOver();
}

void Interacciones::explosionAt(int x, int y) {
    qDebug() << "Explosion at" << x << y;
    // implementar animacion/efecto
}

void Interacciones::win() {
    qDebug() << "WIN!";
    // implementar logica de victoria
}

void Interacciones::gameOver() {
    game_over = true;
    emit gameOverSignal();
}

void Interacciones::arrestado() {
    qDebug() << "ARRESTED action";
    // ejemplo: restar vida, parar movimiento
}

void Interacciones::quitarVidaVisual() {
    // placeholder para UI
}
