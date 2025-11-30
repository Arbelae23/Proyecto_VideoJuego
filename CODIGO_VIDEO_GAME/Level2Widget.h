#ifndef LEVEL2WIDGET_H
#define LEVEL2WIDGET_H

#include <QWidget>
#include <QTimer>
#include "Enemigos.h"
#include "Jugador_R_L.h"
#include "Interacciones.h"
#include "Media.h"
#include <vector>

class Level2Widget : public QWidget {
    Q_OBJECT
public:
    explicit Level2Widget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onTick();

private:
    QTimer timer;
    double dt;
    double t_global;
    bool enemigosCreados = false;

    Jugador_R_L jugador; // en este nivel jugador no se mueve verticalmente, solo lateral
    Interacciones inter;
    Media media;

    std::vector<Enemigos> enemigos; // un seno y un orbital (pueden entrar/salir)
    void setupEnemies();
    void checkCollisions();

protected:

};

#endif // LEVEL2WIDGET_H
