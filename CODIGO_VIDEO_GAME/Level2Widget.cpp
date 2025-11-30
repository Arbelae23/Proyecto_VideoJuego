#include "Level2Widget.h"
#include <QPainter>
#include <QtMath>
#include <cstdlib>

Level2Widget::Level2Widget(QWidget *parent)
    : QWidget(parent), dt(0.016), t_global(0)
{
    setFocusPolicy(Qt::NoFocus);

    jugador.rect = QRect((width()/2)-25, height()-140, 50, 80);
    jugador.speed = 8;
    inter.contador_vidas = 3;

    media.cargarMedia();
    // Crear enemigos una vez que el widget esté listo (sin usar resizeEvent)
    QTimer::singleShot(0, this, [this]{
        if (!enemigosCreados) {
            setupEnemies();
            enemigosCreados = true;
        }
    });

    connect(&timer, &QTimer::timeout, this, &Level2Widget::onTick);
    timer.start(int(dt*1000));
}



// ---------------------------------------------------------------------
void Level2Widget::setupEnemies() {
    enemigos.clear();

    // ---- 3 enemigos sinusoidales ----
    for (int i = 0; i < 3; i++) {
        Enemigos e;
        e.activo = true;
        e.tipo_movimiento = Enemigos::TM_SenoDown;
        e.tamaño = QSize(40,40);

        // ahora width(), height() son correctos
        e.pos_base.setX(rand() % width());
        e.pos_base.setY(rand() % (height()/2));

        e.velocidad = QPointF(50 + rand()%50, 50 + rand()%50);
        e.amplitud_seno = 50 + rand()%50;
        e.frecuencia_seno = 2.0 + (rand()%3);
        e.bounds = QRect(int(e.pos_base.x()), int(e.pos_base.y()),
                         e.tamaño.width(), e.tamaño.height());

        enemigos.push_back(e);
    }

    // ---- 4 enemigos espirales ----
    for (int i = 0; i < 4; i++) {
        Enemigos e;
        e.activo = true;
        e.tipo_movimiento = Enemigos::TM_Espiral;
        e.tamaño = QSize(40,40);

        e.pos_base.setX(rand() % width());
        e.pos_base.setY(rand() % (height()/2));

        e.velocidad = QPointF(0, 50 + rand()%50);
        e.radio_actual = 20 + rand()%20;
        e.velAngular = 2.0 + ((rand()%100)/50.0);
        e.expansion = 20 + rand()%20;

        e.bounds = QRect(int(e.pos_base.x()), int(e.pos_base.y()),
                         e.tamaño.width(), e.tamaño.height());

        enemigos.push_back(e);
    }
}


// ---------------------------------------------------------------------
void Level2Widget::onTick() {
    double sec = dt;

    for (auto &e : enemigos) {
        e.update(sec, width(), height());
    }

    checkCollisions();
    update();
}


// ---------------------------------------------------------------------
void Level2Widget::checkCollisions() {
    for (auto &e : enemigos) {
        if (!e.activo) continue;

        if (jugador.getBounds().intersects(e.bounds)) {
            inter.quitar_vida(1);
            media.reproducir_sonidoChoque();
            e.activo = false;
        }
    }
    jugador.vidas = inter.contador_vidas;
}


// ---------------------------------------------------------------------
void Level2Widget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), QColor(230,235,255));

    // suelo
    p.setBrush(QColor(180,180,180));
    p.drawRect(0, height()-60, width(), 60);

    // jugador
    jugador.draw(p);

    // enemigos
    for (auto &e : enemigos) e.draw(p);

    // HUD
    p.setPen(Qt::black);
    p.drawText(10,20, QString("Vidas: %1").arg(jugador.vidas));
}
