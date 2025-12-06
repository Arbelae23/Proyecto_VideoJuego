#include "Level3Widget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QShowEvent>
#include <cstdlib>
#include <algorithm>
#include <cmath>

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
        const int h = 220;  // alto aumentado
        jugador.rect = QRect(
            roadX + roadWidth/2 - w/2,
            0,
            w,
            h
        );
    }
    jugador.speed = 24; // aumentar velocidad lateral del jugador en nivel 3
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

    // Gracia de 3 segundos: se activará al mostrar el nivel (showEvent)
    graceActive = false;
    graceMsRemaining = 3000;
    firstSpawnDone = false;

    // Crear enemigos iniciales inmediatamente (visibles pero sin daño durante la gracia)
    spawnObstacles();

    connect(&timer, &QTimer::timeout, this, &Level3Widget::onTick);
    timer.start(int(dt*1000));
}

void Level3Widget::spawnObstacles() {
    enemigos.clear();
    // Crear 3 autos, uno por cada carril fijo
    const int laneXs[3] = { laneX1, laneX2, laneX3 };
    const int sepY = 150; // separación vertical fija entre autos
    for (int i=0;i<3;i++) {
        Enemigos e;
        // Mantener activos (se verán y se moverán)
        e.activo = true;
        e.tipo_movimiento = Enemigos::TM_Linear;
        e.lane = i; // fijar carril para este enemigo
        int spawnX = laneXs[i];
        e.pos_f = QPointF(spawnX, laneSpawnY - i*sepY);
        e.pos_inicial = e.pos_f; // guardar Y inicial para cálculo de crecimiento
        e.velocidad = QPointF(0, 150); // velocidad vertical hacia abajo
        // Tamaño inicial mínimo (consistente con minH=24 en onTick)
        e.tamaño = QSize(40, 24);
        // Asignar sprite aleatorio entre 3 opciones
        e.usaSprite = true;
        switch (std::rand() % 3) {
            case 0: e.sprite = QPixmap(media.lvl3_carro1); break;
            case 1: e.sprite = QPixmap(media.lvl3_carro2); break;
            default: e.sprite = QPixmap(media.lvl3_carro3); break;
        }
        e.spriteNormal = e.sprite;
        e.bounds = QRect(int(e.pos_f.x()), int(e.pos_f.y()), e.tamaño.width(), e.tamaño.height());
        enemigos.push_back(e);
    }
}

void Level3Widget::onTick() {
    double sec = dt;
    // Gestionar periodo de gracia: sólo desactiva el daño por colisión
    if (graceActive) {
        graceMsRemaining -= int(dt * 1000);
        if (graceMsRemaining <= 0) {
            graceActive = false;
        }
    }
    // Movimiento continuo del jugador basado en teclas mantenidas
    double dx = 0.0;
    const double playerSpeedPxSec = 600.0; // velocidad lateral alta en px/s
    if (leftHeld)  dx -= playerSpeedPxSec * sec;
    if (rightHeld) dx += playerSpeedPxSec * sec;
    if (dx != 0.0) {
        jugador.moverPorVector(float(dx), 0.0f);
        // Clamp a límites de la carretera
        if (jugador.getBounds().left() < roadX)
            jugador.rect.moveLeft(roadX);
        const int rightBound = roadX + roadWidth;
        if (jugador.getBounds().right() > rightBound)
            jugador.rect.moveRight(rightBound);
    }
    for (auto &e : enemigos) {
        if (!e.activo) continue;
        if (e.tipo_movimiento == Enemigos::TM_Linear) {
            // Movimiento vertical principal
            e.pos_f.setY(e.pos_f.y() + e.velocidad.y() * sec);

            // Deriva lateral según carril
            const double lateralSpeed = 220.0; // px/s, más notorio
            int leftBoundX = roadX;
            int rightBoundX = roadX + roadWidth - e.tamaño.width();
            if (e.lane == 0) {
                e.pos_f.setX(std::max<double>(leftBoundX, e.pos_f.x() - lateralSpeed * sec));
            } else if (e.lane == 2) {
                e.pos_f.setX(std::min<double>(rightBoundX, e.pos_f.x() + lateralSpeed * sec));
            }

            // Escalado por perspectiva: crecer al bajar
            const double minH = 15.0;      // altura mínima al aparecer (más pequeño)
            const double maxH = 330.0;     // altura máxima cerca del jugador
            const double aspect = 60.0/36.0; // relación ancho/alto base
            double range = (height() - e.pos_inicial.y()) + 120.0; // desde su spawn hasta abajo
            if (range < 1.0) range = 1.0;
            double norm = (e.pos_f.y() - e.pos_inicial.y()) / range; // 0 en spawn, ~1 al fondo
            norm = std::clamp(norm, 0.0, 1.0);
            double curve = std::pow(norm, 1.4); // crecimiento acelerado hacia el final
            int newH = int(minH + curve * (maxH - minH));
            int newW = int(newH * aspect);
            e.tamaño = QSize(newW, newH);
            e.bounds.setSize(e.tamaño);
            // si sale por abajo, respawnear por arriba en un carril aleatorio
            if (e.pos_f.y() > height() + 80) {
                const int laneXs[3] = { laneX1, laneX2, laneX3 };
                e.pos_f.setY(laneSpawnY);
                int laneIndex = (e.lane >= 0 && e.lane < 3) ? e.lane : 0;
                e.pos_f.setX(laneXs[laneIndex]);
                e.pos_inicial = e.pos_f; // reiniciar referencia de crecimiento al nuevo spawn
                // Reiniciar tamaño mínimo inmediato al respawn
                e.tamaño = QSize(int(minH * aspect), int(minH));
                e.bounds.setSize(e.tamaño);
                // Cambiar sprite aleatoriamente en cada respawn (opcional)
                switch (std::rand() % 3) {
                    case 0: e.sprite = QPixmap(media.lvl3_carro1); break;
                    case 1: e.sprite = QPixmap(media.lvl3_carro2); break;
                    default: e.sprite = QPixmap(media.lvl3_carro3); break;
                }
                e.spriteNormal = e.sprite;
            }
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
        if (graceActive) continue; // Durante la gracia, no se pierde vida
        if (!collisionEnabledL3) continue; // opción para deshabilitar colisiones en L3

        // Hitbox del jugador, recortado adicionalmente sólo para Nivel 3
        QRect playerHB = jugador.getHitbox();
        int extraX = int(playerHB.width() * l3ExtraInsetXRatio);
        int extraY = int(playerHB.height() * l3ExtraInsetYRatio);
        if (extraX > 0 || extraY > 0) playerHB = playerHB.adjusted(extraX, extraY, -extraX, -extraY);

        QRect enemyB = e.getBounds();
        QRect overlap = playerHB.intersected(enemyB);
        // Exigir un solapamiento con área suficiente para considerar choque real
        double minArea = l3MinOverlapAreaRatio * (enemyB.width() * enemyB.height());
        if (!(overlap.isNull()) && (overlap.width() * overlap.height()) >= minArea) {
            // Segunda verificación: escala similar (misma profundidad)
            const int expectedMaxEnemyH = 140;
            const int tolerancePx = 20;
            bool similarScale = std::abs(e.getBounds().height() - expectedMaxEnemyH) <= tolerancePx;
            if (!similarScale) continue;
            inter.quitar_vida(1);
            media.reproducir_sonidoChoque();
            // respawn inmediato del auto en un carril aleatorio
            const int laneXs[3] = { laneX1, laneX2, laneX3 };
            e.pos_f.setY(laneSpawnY);
            int laneIndex = (e.lane >= 0 && e.lane < 3) ? e.lane : 0;
            e.pos_f.setX(laneXs[laneIndex]);
            e.pos_inicial = e.pos_f; // reiniciar referencia de crecimiento
            // Reiniciar tamaño mínimo inmediato al respawn por choque
            const double minH2 = 24.0;
            const double aspect2 = 60.0/36.0;
            e.tamaño = QSize(int(minH2 * aspect2), int(minH2));
            e.bounds.setSize(e.tamaño);
            // Cambiar sprite aleatoriamente en cada respawn por choque (opcional)
            switch (std::rand() % 3) {
                case 0: e.sprite = QPixmap(media.lvl3_carro1); break;
                case 1: e.sprite = QPixmap(media.lvl3_carro2); break;
                default: e.sprite = QPixmap(media.lvl3_carro3); break;
            }
            e.spriteNormal = e.sprite;
            e.bounds.moveTopLeft(QPoint(int(e.pos_f.x()), int(e.pos_f.y())));
        }
    }
    jugador.vidas = inter.contador_vidas;
}

void Level3Widget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    // Activar la gracia al abrir el nivel 3
    graceActive = true;
    graceMsRemaining = 3000; // 3 segundos
    firstSpawnDone = true; // ya hay enemigos en pantalla
}

void Level3Widget::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key == Qt::Key_A || key == Qt::Key_Left) {
        desiredFrame = -2;
        leftHeld = true;
    }
    else if (key == Qt::Key_D || key == Qt::Key_Right) {
        desiredFrame = 2;
        rightHeld = true;
    }

    update();
}

void Level3Widget::keyReleaseEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key == Qt::Key_A || key == Qt::Key_Left || key == Qt::Key_D || key == Qt::Key_Right) {
        desiredFrame = 0;
        if (key == Qt::Key_A || key == Qt::Key_Left) leftHeld = false;
        if (key == Qt::Key_D || key == Qt::Key_Right) rightHeld = false;
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

    // dibujar enemigos (autos) primero para que queden DETRÁS del jugador
    for (auto &e : enemigos) {
        e.draw(p);
    }

    // dibujar jugador (moto) al final para que quede POR ENCIMA
    jugador.draw(p);

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
