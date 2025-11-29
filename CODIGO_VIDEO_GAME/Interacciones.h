#ifndef INTERACCIONES_H
#define INTERACCIONES_H

#include <QObject>

class Interacciones : public QObject {
    Q_OBJECT
public:
    explicit Interacciones(QObject *parent = nullptr);

    // Atributos
    int trofeos;
    int contador_vidas;
    int cuenta_atras; // segundos
    bool game_over;

    // Métodos
    void sumar_Trofeos(int n = 1);
    void quitar_vida(int n = 1);
    void explosionAt(int x, int y);
    void win();
    void gameOver();
    void arrestado();
    void quitarVidaVisual();

signals:
    void vidasChanged(int v);
    void trofeosChanged(int t);
    void gameOverSignal();
};

#endif // INTERACCIONES_H
