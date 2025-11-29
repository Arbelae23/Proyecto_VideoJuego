#ifndef LEVEL3WIDGET_H
#define LEVEL3WIDGET_H

#include <QWidget>
#include <QTimer>
#include "Jugador_R_L.h"
#include "Enemigos.h"
#include "Interacciones.h"
#include "Media.h"
#include <vector>

class Level3Widget : public QWidget {
    Q_OBJECT
public:
    explicit Level3Widget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onTick();

private:
    QTimer timer;
    double dt;

    Jugador_R_L jugador; // representará la moto desde atrás
    Interacciones inter;
    Media media;

    std::vector<Enemigos> enemigos;
    void spawnObstacles();
    void checkCollisions();
};

#endif // LEVEL3WIDGET_H
