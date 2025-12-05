#include "Level2Widget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QtMath>
#include <cstdlib>
#include <QDebug>

Level2Widget::Level2Widget(QWidget *parent)
    : QWidget(parent), dt(0.016), t_global(0)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // cargar media primero (IMPORTANTE)
    media.cargarMedia();

    // --- Inicializar jugador ---
    jugador.rect = QRect(200, 350, 50, 80);
    jugador.speed = 8;
    jugador.vidas = 3;

    // cargar background desde media (después de cargar media)
    if (!media.background_nivel2.isEmpty()) {
        background.load(media.background_nivel2);
        if (background.isNull()) qDebug() << "Level2: background failed to load:" << media.background_nivel2;
    } else {
        qDebug() << "Level2: media.background_nivel2 está vacío";
    }

    // Asignar sprite del jugador desde Media (jugador 2)
    currentFrame = 0;
    desiredFrame = 0;
    jugador.skin = media.jugador2_sprite0;

    enemigosCreados = false;

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
    for (int i = 0; i < 3; i++)
    {
        Enemigos e;
        e.activo = true;
        e.tipo_movimiento = Enemigos::TM_SenoDown;
        e.tamaño = QSize(120, 80);

        // ✅ ESPACIADO REAL EN TODO EL ANCHO
        int separacionX = width() / 3;
        e.pos_base.setX(i * separacionX + separacionX /2);

        // ✅ ESPACIADO VERTICAL CONTROLADO
        e.pos_base.setY(50 + i * 100);

        // ✅ VELOCIDADES DISTINTAS
        e.velocidad = QPointF(0, 100 + i * 40 + rand() % 30);

        // ✅ SENO DISTINTO PARA CADA UNO
        e.amplitud_seno = 50 + i * 15;
        e.frecuencia_seno = 1.5 + (i * 0.4);

        e.bounds = QRect(
            int(e.pos_base.x()),
            int(e.pos_base.y()),
            e.tamaño.width(),
            e.tamaño.height()
            );

        // ✅ SPRITE
        if (!media.policia_sprite.isEmpty()) {
            e.sprite.load(media.policia_sprite);
            if (!e.sprite.isNull())
                e.usaSprite = true;
        }

        e.pos_f = e.pos_base;
        enemigos.push_back(e);
    }



    // ----------------- ENEMIGOS EN ESPIRAL HORIZONTAL -----------------
    for (int i = 0; i < 2; i++)
    {
        Enemigos e;
        e.activo = true;
        e.tipo_movimiento = Enemigos::TM_EspiralHorizontal;

        e.tamaño = QSize(120, 80);

        // ✅ SEPARACIÓN HORIZONTAL MÁS AMPLIA
        int separacionX = 1000;   // antes 150 (muy juntos)
        e.pos_base.setX(-150 - i * separacionX);

        // ✅ SEPARACIÓN VERTICAL CONTROLADA
        e.pos_base.setY(140 + i * 300);

        // ✅ VELOCIDAD DIFERENTE PARA CADA UNO
        int velBase = 140;
        int variacion = rand() % 80;   // velocidades distintas
        e.velocidad = QPointF(velBase + variacion, 0);

        // ✅ PARÁMETROS DISTINTOS DE ESPIRAL
        e.radio_actual = 30 + rand() % 40;
        e.velAngular = 4 + rand() % 4;

        e.bounds = QRect(
            int(e.pos_base.x()),
            int(e.pos_base.y()),
            e.tamaño.width(),
            e.tamaño.height()
            );

        // ✅ Sprite del policía (bicicleta)
        if (!media.bicicleta_sprite.isEmpty()) {
            e.sprite.load(media.bicicleta_sprite);
            if (!e.sprite.isNull()) {
                e.usaSprite = true;
            }
        }

        e.pos_f = e.pos_base;
        enemigos.push_back(e);
    }

}

void Level2Widget::keyPressEvent(QKeyEvent *ev)
{
    if (ev->isAutoRepeat()) return;

    switch (ev->key())
    {
    case Qt::Key_W:
        wPressed = true;
        desiredFrame = 0;   // ARRIBA
        break;

    case Qt::Key_S:
        sPressed = true;
        desiredFrame = 4;   // ABAJO
        break;

    case Qt::Key_A:
        aPressed = true;
        desiredFrame = 6;   // IZQUIERDA
        break;

    case Qt::Key_D:
        dPressed = true;
        desiredFrame = 2;   // DERECHA
        break;
    default:
        QWidget::keyPressEvent(ev);
        break;
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
    default:
        QWidget::keyReleaseEvent(ev);
        break;
    }
}

void Level2Widget::moverJugadorWASD()
{
    float dx = 0, dy = 0;

    if (wPressed) dy -= 1;
    if (sPressed) dy += 1;
    if (aPressed) dx -= 1;
    if (dPressed) dx += 1;

    // ---- SELECCIÓN DE SPRITE PARA DIAGONALES ----
    if (wPressed && dPressed) desiredFrame = 1; // WD
    else if (dPressed && sPressed) desiredFrame = 3; // SD
    else if (sPressed && aPressed) desiredFrame = 5; // SA
    else if (aPressed && wPressed) desiredFrame = 7; // WA
    else if (wPressed) desiredFrame = 0; // Arriba
    else if (dPressed) desiredFrame = 2; // Derecha
    else if (sPressed) desiredFrame = 4; // Abajo
    else if (aPressed) desiredFrame = 6; // Izquierda

    // ---- MOVIMIENTO NORMALIZADO ----
    if (dx != 0 || dy != 0)
    {
        float len = std::sqrt(dx * dx + dy * dy);
        dx = (dx / len) * jugador.speed;
        dy = (dy / len) * jugador.speed;
        jugador.moverPorVector(dx, dy);

        // evitar salir de pantalla
        if (jugador.getBounds().left() < 0) jugador.rect.moveLeft(0);
        if (jugador.getBounds().right() > width()) jugador.rect.moveRight(width() - jugador.rect.width());
        if (jugador.getBounds().top() < 0) jugador.rect.moveTop(0);
        if (jugador.getBounds().bottom() > height()) jugador.rect.moveBottom(height() - jugador.rect.height());
    }
}

void Level2Widget::onTick()
{
    moverJugadorWASD();

    // --- ANIMACIÓN SUAVE DE SPRITES (circular 0..7) ---
    animAccumulatorMs += int(dt * 1000);
    if (animAccumulatorMs >= animStepMs)
    {
        int totalFrames = 8;

        int diff = desiredFrame - currentFrame;

        // envolver en ciclo circular [-4, +4]
        if (diff >  totalFrames / 2) diff -= totalFrames;
        if (diff < -totalFrames / 2) diff += totalFrames;

        if (diff > 0)      currentFrame++;
        else if (diff < 0) currentFrame--;

        // normalizar entre 0 y 7
        if (currentFrame < 0) currentFrame += totalFrames;
        if (currentFrame >= totalFrames) currentFrame -= totalFrames;

        updatePlayerSkin();
        animAccumulatorMs = 0;
    }

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

void Level2Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    // FONDO
    if (!background.isNull()) {
        p.drawPixmap(rect(), background, background.rect());
    } else {
        p.fillRect(rect(), QColor(240,240,240));
    }

    // AJUSTE DE POSICIÓN DEL JUGADOR
    if (jugador.rect.top() == 100) {
        jugador.rect.moveTop(height() - 140);
    }

    //  DIBUJAR ENEMIGOS
    for (auto &e : enemigos) {
        e.draw(p);
    }

    // JUGADOR ENCIMA DE LOS ENEMIGOS
    jugador.draw(p);

    // HUD
    p.setPen(Qt::black);
    p.drawText(10,20, QString("Vidas: %1").arg(jugador.vidas));
}

void Level2Widget::updatePlayerSkin()
{
    switch (currentFrame)
    {
    case 0: jugador.skin = media.jugador2_sprite0; break; // W
    case 1: jugador.skin = media.jugador2_sprite1; break; // WD
    case 2: jugador.skin = media.jugador2_sprite2; break; // D
    case 3: jugador.skin = media.jugador2_sprite3; break; // SD
    case 4: jugador.skin = media.jugador2_sprite4; break; // S
    case 5: jugador.skin = media.jugador2_sprite5; break; // SA
    case 6: jugador.skin = media.jugador2_sprite6; break; // A
    case 7: jugador.skin = media.jugador2_sprite7; break; // WA
    default: jugador.skin = media.jugador2_sprite0; break;
    }
}
