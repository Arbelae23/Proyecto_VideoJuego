#ifndef LEVEL1WIDGET_H
#define LEVEL1WIDGET_H

#include <QWidget>
#include <QTimer>
#include "Jugador_R_L.h"
#include "Enemigos.h"
#include "Interacciones.h"
#include "Media.h"
#include <vector>

class Level1Widget : public QWidget {
    Q_OBJECT
public:
    explicit Level1Widget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onTick();

private:
    QTimer timer;
    double dt;

    Jugador_R_L jugador;
    Interacciones inter;
    Media media;

    std::vector<Enemigos> enemigos; // obstáculos móviles simples
    void spawnEnemies();
    void checkCollisions();
};

#endif // LEVEL1WIDGET_H
