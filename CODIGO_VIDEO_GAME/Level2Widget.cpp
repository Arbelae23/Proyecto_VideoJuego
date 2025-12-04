#include "Level2Widget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QtMath>
#include <cstdlib>

Level2Widget::Level2Widget(QWidget *parent)
    : QWidget(parent), dt(0.016), t_global(0)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // --- Inicializar jugador ---
    jugador.rect = QRect(200, 350, 50, 80);
    jugador.speed = 8;
    jugador.vidas = 3;
    background.load(media.background_nivel2);

    // --- Interacciones ---
    inter.contador_vidas = 3;

    // Timer
    connect(&timer, &QTimer::timeout, this, &Level2Widget::onTick);
    timer.start(int(dt * 1000));
}

void Level2Widget::resizeEvent(QResizeEvent *ev)
{
    QWidget::resizeEvent(ev);

    if (!enemigosCreados)
    {
        setupEnemies();
        enemigosCreados = true;
    }
}

void Level2Widget::setupEnemies()
{
    enemigos.clear();

    for (int i = 0; i < 4; i++)
    {
        Enemigos e;
        e.activo = true;
        e.tipo_movimiento = Enemigos::TM_SenoDown;
        e.tamaño = QSize(40, 40);

        e.pos_base.setX(rand() % width());
        e.pos_base.setY(rand() % (height() / 2));

        e.velocidad = QPointF(50 + rand() % 50, 50 + rand() % 50);
        e.amplitud_seno = 50 + rand() % 50;
        e.frecuencia_seno = 2 + rand() % 3;

        e.bounds = QRect(e.pos_base.x(), e.pos_base.y(),
                         e.tamaño.width(), e.tamaño.height());

        enemigos.push_back(e);
    }
}

void Level2Widget::keyPressEvent(QKeyEvent *ev)
{
    if (ev->isAutoRepeat()) return;

    switch (ev->key())
    {
    case Qt::Key_W: wPressed = true; break;
    case Qt::Key_A: aPressed = true; break;
    case Qt::Key_S: sPressed = true; break;
    case Qt::Key_D: dPressed = true; break;
    }
}

void Level2Widget::keyReleaseEvent(QKeyEvent *ev)
{
    if (ev->isAutoRepeat()) return;

    switch (ev->key())
    {
    case Qt::Key_W: wPressed = false; break;
    case Qt::Key_A: aPressed = false; break;
    case Qt::Key_S: sPressed = false; break;
    case Qt::Key_D: dPressed = false; break;
    }
}

void Level2Widget::moverJugadorWASD()
{
    float dx = 0, dy = 0;

    if (wPressed) dy -= 1;
    if (sPressed) dy += 1;
    if (aPressed) dx -= 1;
    if (dPressed) dx += 1;

    if (dx != 0 || dy != 0)
    {
        float len = std::sqrt(dx * dx + dy * dy);
        dx = (dx / len) * jugador.speed;
        dy = (dy / len) * jugador.speed;

        jugador.moverPorVector(dx, dy);
    }
}

void Level2Widget::onTick()
{
    moverJugadorWASD();

    // Actualizar enemigos
    for (auto &e : enemigos)
        e.update(dt, width(), height());

    checkCollisions();
    update();
}

void Level2Widget::checkCollisions()
{
    for (auto &e : enemigos)
    {
        if (!e.activo) continue;

        if (jugador.getBounds().intersects(e.bounds))
        {
            inter.quitar_vida(1);

            jugador.vidas = inter.contador_vidas;
            e.activo = false;
        }
    }
}

void Level2Widget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    // Dibujar fondo si está disponible, de lo contrario usar color plano
    if (!background.isNull()) {
        p.drawPixmap(rect(), background, background.rect());
        //p.drawPixmap(QRect(0, 0, 1600, 900), background, background.rect());
    } else {
        p.fillRect(rect(), QColor(240,240,240));
    }

    // actualizar jugador dibujo (asegurar su Y en resize)
    if (jugador.rect.top() == 100) { // posición por defecto; forzar a bottom on first draw
        jugador.rect.moveTop(height() - 140);
    }

    // dibujar jugador
    jugador.draw(p);
}


