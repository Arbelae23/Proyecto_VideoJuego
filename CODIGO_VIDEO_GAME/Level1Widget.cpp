#include "Level1Widget.h"
#include <QPainter>
#include <QKeyEvent>
#include <Enemigos.h>

Level1Widget::Level1Widget(QWidget *parent)
    : QWidget(parent), dt(0.016)
{
    setFocusPolicy(Qt::StrongFocus);
    // inicializar jugador en centro abajo
    jugador.rect = QRect( (width()/2) - 25, height() - 140, 50, 80 );
    jugador.vidas = 3;
    jugador.speed = 10;

    inter.contador_vidas = jugador.vidas;
    media.cargarMedia();
    // Cargar imagen de fondo usando la ruta definida en Media
    background.load(media.background_nivel);
    // Configurar velocidad de desplazamiento del camino (px/seg)
    media.setRoadSpeed(240);
    media.setRoadDirectionDown(false); // mover hacia arriba
    // Asignar sprite del jugador desde Media (jugador 1)
    jugador.skin = media.jugador1_sprite;
    // Ajustar tamaño del jugador (hitbox) basado en el sprite y escalarlo más grande
    {
        QPixmap carPix(media.jugador1_sprite);
        if (!carPix.isNull()) {
            const double scale = 0.5; // hacer el coche más grande
            const int newW = qRound(carPix.width() * scale);
            const int newH = qRound(carPix.height() * scale);
            const int floorH = 60;    // altura del suelo dibujado
            const int margin = 20;    // margen sobre el suelo
            jugador.rect = QRect(
                (width()/2) - newW/2,
                height() - floorH - margin - newH,
                newW,
                newH
            );
        }
    }
    // spawn algunos enemigos (lineales desde la izquierda)
    spawnEnemies();

    connect(&timer, &QTimer::timeout, this, &Level1Widget::onTick);
    timer.start(int(dt*1000));
}

void Level1Widget::spawnEnemies() {
    enemigos.clear();
    for (int i=0;i<5;i++) {
        Enemigos e;
        e.activo = true;
        e.tipo_movimiento = Enemigos::TM_Linear;
        e.pos_f = QPointF(50 + i * 100, -200 - i*150); // ARRIBA de la pantalla
        e.velocidad = QPointF(0, 150 + i*20);          // velocidad hacia abajo
        e.tamaño = QSize(40,40);
        e.bounds = QRect(int(e.pos_f.x()), int(e.pos_f.y()), 40, 40);
        enemigos.push_back(e);
    }
}


void Level1Widget::onTick() {
    double sec = dt;
    // actualizar enemigos (lineales)
    for (auto &e : enemigos) {
        if (!e.activo) continue;
        if (e.tipo_movimiento == Enemigos::TM_Linear) {
            // Movimiento vertical: BAJAR
            e.pos_f.setY(e.pos_f.y() + e.velocidad.y() * sec);

            // Respawn arriba si sale por abajo
            if (e.pos_f.y() > height() + 50) {
                e.pos_f.setY(-100);              // reaparece arriba
                e.pos_f.setX(rand() % width());  // posición aleatoria horizontal
            }

            e.bounds.moveTopLeft(QPoint(int(e.pos_f.x()), int(e.pos_f.y())));
        }
    }

    checkCollisions();
    update();
}

void Level1Widget::checkCollisions() {
    for (auto &e : enemigos) {
        if (!e.activo) continue;
        if (jugador.getBounds().intersects(e.getBounds())) {
            // colisión: quitar vida y resetear enemigo
            inter.quitar_vida(1);
            media.reproducir_sonidoChoque();
            // respawn enemigo fuera de pantalla
            e.pos_f.setX(-120);
            e.bounds.moveTopLeft(QPoint(int(e.pos_f.x()), int(e.pos_f.y())));
        }
    }
    // sincronizar vidas
    jugador.vidas = inter.contador_vidas;
}

void Level1Widget::paintEvent(QPaintEvent *) {
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

    // Dibujar camino desplazándose (entre background y jugador)
    media.drawScrollingRoad(p, rect(), dt);

    // dibujar jugador
    jugador.draw(p);

    // dibujar enemigos
    for (auto &e : enemigos) e.draw(p);

    // HUD vidas
    p.setPen(Qt::black);
    p.drawText(10,20, QString("Vidas: %1").arg(jugador.vidas));
}

void Level1Widget::keyPressEvent(QKeyEvent *event) {
    const int key = event->key();
    if (key == Qt::Key_A || key == Qt::Key_Left) {
        jugador.moverIzquierda();
        if (jugador.getBounds().left() < 0) jugador.rect.moveLeft(0);
    } else if (key == Qt::Key_D || key == Qt::Key_Right) {
        jugador.moverDerecha();
        if (jugador.getBounds().right() > width()) jugador.rect.moveRight(width() - jugador.rect.width());
    }
    update();
}
