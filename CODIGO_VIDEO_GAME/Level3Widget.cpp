#include "Level3Widget.h"
#include <QPainter>
#include <QKeyEvent>

Level3Widget::Level3Widget(QWidget *parent)
    : QWidget(parent), dt(0.016)
{
    setFocusPolicy(Qt::StrongFocus);
    // Valores fijos para la carretera (modifica a tu gusto)
    roadX = 180;        // px desde la izquierda
    roadWidth = 1000;    // px de ancho

    // Inicializar jugador centrado en la carretera, más abajo (margen inferior fijo)
    {
        // Inicial posicion sólo para tener ancho/alto; ajustamos vertical en paintEvent
        const int w = 120;   // ancho aumentado
        const int h = 320;  // alto aumentado
        jugador.rect = QRect(
            roadX + roadWidth/2 - w/2,
            0,
            w,
            h
        );
    }
    jugador.speed = 12;
    inter.contador_vidas = 3;
    media.cargarMedia();
    // Cargar imagen de fondo de Nivel 3 desde Media
    background.load(media.background_nivel3);

    // Estado inicial de animación y sprite (idle)
    currentFrame = 0;
    desiredFrame = 0;
    jugador.skin = media.jugador3_sprite0;

    // Paso de animación fijo (ms)
    animStepMs = 83;

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

    // Temporizar la transición de animación hacia el objetivo
    animAccumulatorMs += int(dt * 1000);
    if (animAccumulatorMs >= animStepMs) {
        if (currentFrame < desiredFrame) {
            currentFrame++;
        } else if (currentFrame > desiredFrame) {
            currentFrame--;
        }
        animAccumulatorMs = 0;
        updatePlayerSkin();
    }
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
    const int key = event->key();
    if (key == Qt::Key_A || key == Qt::Key_Left) {
        desiredFrame = -2;
        jugador.moverIzquierda();
        if (jugador.getBounds().left() < roadX)
            jugador.rect.moveLeft(roadX);
    }
    else if (key == Qt::Key_D || key == Qt::Key_Right) {
        desiredFrame = 2;
        jugador.moverDerecha();
        const int rightBound = roadX + roadWidth;
        if (jugador.getBounds().right() > rightBound)
            jugador.rect.moveRight(rightBound - jugador.rect.width());
    }

    update();
}

void Level3Widget::keyReleaseEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key == Qt::Key_A || key == Qt::Key_Left || key == Qt::Key_D || key == Qt::Key_Right) {
        desiredFrame = 0;
    }
    update();
}


void Level3Widget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    // Dibujar fondo si está disponible, de lo contrario usar color plano
    if (!background.isNull()) {
        p.drawPixmap(rect(), background, background.rect());
    } else {
        p.fillRect(rect(), QColor(210,240,255));
    }

    // Asegurar jugador más abajo cuando el widget ya tiene tamaño válido
    if (!playerPosInitialized) {
        const int w = jugador.rect.width();
        const int h = jugador.rect.height();
        int marginBottom = 2; // ajusta este valor para colocarlo más abajo
        int y = height() - marginBottom - h;
        if (y < 0) y = 0;
        if (y > height() - h) y = height() - h;
        jugador.rect.moveTo(roadX + roadWidth/2 - w/2, y);
        playerPosInitialized = true;
    }

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

void Level3Widget::updatePlayerSkin() {
    switch (currentFrame) {
        case -2:
            jugador.skin = media.jugador3_sprite_2;
            break;
        case -1:
            jugador.skin = media.jugador3_sprite_1;
            break;
        case 0:
            jugador.skin = media.jugador3_sprite0;
            break;
        case 1:
            jugador.skin = media.jugador3_sprite1;
            break;
        case 2:
            jugador.skin = media.jugador3_sprite2;
            break;
        default:
            jugador.skin = media.jugador3_sprite0;
            break;
    }
}
