#include "Level3Widget.h"
#include <QPainter>
#include <QKeyEvent>

Level3Widget::Level3Widget(QWidget *parent)
    : QWidget(parent), dt(0.016)
{
    setFocusPolicy(Qt::StrongFocus);
    jugador.rect = QRect( (width()/2)-20, height()-170, 40, 120 );
    jugador.speed = 12;
    inter.contador_vidas = 3;
    media.cargarMedia();

    spawnObstacles();

    connect(&timer, &QTimer::timeout, this, &Level3Widget::onTick);
    timer.start(int(dt*1000));
}

void Level3Widget::spawnObstacles() {
    enemigos.clear();
    // crear varios autos que bajan por la pantalla (y respawnean por arriba)
    for (int i=0;i<5;i++) {
        Enemigos e;
        e.activo = true;
        e.tipo_movimiento = Enemigos::TM_Linear;
        e.pos_f = QPointF(50 + i*100, -100 - i*120);
        e.velocidad = QPointF(0, 120 + i*30); // movimiento vertical hacia abajo
        e.tamaño = QSize(50, 30);
        e.bounds = QRect(int(e.pos_f.x()), int(e.pos_f.y()), e.tamaño.width(), e.tamaño.height());
        enemigos.push_back(e);
    }
}

void Level3Widget::onTick() {
    double sec = dt;
    for (auto &e : enemigos) {
        if (!e.activo) continue;
        if (e.tipo_movimiento == Enemigos::TM_Linear) {
            e.pos_f.setY(e.pos_f.y() + e.velocidad.y() * sec);
            // si sale por abajo respawnear por arriba
            if (e.pos_f.y() > height() + 80) e.pos_f.setY(-120);
            e.bounds.moveTopLeft(QPoint(int(e.pos_f.x()), int(e.pos_f.y())));
        }
    }
    checkCollisions();
    update();
}

void Level3Widget::checkCollisions() {
    for (auto &e : enemigos) {
        if (!e.activo) continue;
        if (jugador.getBounds().intersects(e.getBounds())) {
            inter.quitar_vida(1);
            media.reproducir_sonidoChoque();
            // respawn inmediato del auto
            e.pos_f.setY(-150);
            e.bounds.moveTopLeft(QPoint(int(e.pos_f.x()), int(e.pos_f.y())));
        }
    }
    jugador.vidas = inter.contador_vidas;
}

void Level3Widget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left) {
        jugador.moverIzquierda();
        if (jugador.getBounds().left() < width()/4)
            jugador.rect.moveLeft(width()/4);
    }
    else if (event->key() == Qt::Key_Right) {
        jugador.moverDerecha();
        if (jugador.getBounds().right() > 3*(width()/4))
            jugador.rect.moveRight(3*(width()/4) - jugador.rect.width());
    }

    update();
}


void Level3Widget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), QColor(210,240,255));
    // dibujar carretera
    p.setBrush(QColor(50,50,50));
    p.drawRect(width()/4, 0, width()/2, height());

    // dibujar jugador (moto)
    jugador.draw(p);

    // dibujar enemigos (autos)
    p.setBrush(Qt::red);
    for (auto &e : enemigos) {
        p.drawRect(e.getBounds());
    }

    // HUD
    p.setPen(Qt::white);
    p.drawText(10,20, QString("Vidas: %1").arg(jugador.vidas));
}
