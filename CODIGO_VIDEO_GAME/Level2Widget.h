#ifndef LEVEL2WIDGET_H
#define LEVEL2WIDGET_H

#include <QWidget>
#include <QTimer>
#include "Jugador_U_D_Diagonals.h"
#include "Enemigos.h"
#include "Interacciones.h"

class Level2Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Level2Widget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;

private slots:
    void onTick();

private:
    // --- JUGADOR ---
    Jugador_U_D_Diagonals jugador;

    // --- ENEMIGOS ---
    std::vector<Enemigos> enemigos;
    void setupEnemies();
    bool enemigosCreados = false;

    // --- INTERACCIONES ---
    Interacciones inter;

    // --- TIMER ---
    QTimer timer;
    double dt;
    double t_global;

    // --- TECLAS ---
    bool wPressed = false;
    bool aPressed = false;
    bool sPressed = false;
    bool dPressed = false;

    // --- METODOS ---
    void moverJugadorWASD();
    void checkCollisions();
};

#endif // LEVEL2WIDGET_H
