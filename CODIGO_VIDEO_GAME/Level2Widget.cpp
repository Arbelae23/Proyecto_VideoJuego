#include "Level2Widget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QtMath>
#include <cstdlib>
#include <QDebug>

Level2Widget::Level2Widget(QWidget *parent)
    : QWidget(parent), dt(0.016), t_global(0)
{
    mostrarGameOver = false;
    esperandoDecision = false;
    enemigosCreados = false;

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    gameOverImg.load(media.Gameover);

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
            e.spriteNormal.load(media.policia_sprite);
            e.sprite = e.spriteNormal;
            e.usaSprite = true;
        }

        if (!media.Choque.isEmpty()) {
            e.spriteChoque.load(media.Choque);
        }


        e.pos_inicial = e.pos_base;
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

        // ✅ Sprite de la bicicleta
        if (!media.bicicleta_sprite.isEmpty()) {
            e.spriteNormal.load(media.bicicleta_sprite);
            e.sprite = e.spriteNormal;
            e.usaSprite = true;
        }

        if (!media.Choque.isEmpty()) {
            e.spriteChoque.load(media.Choque);
        }

        e.pos_inicial = e.pos_base;
        e.pos_f = e.pos_base;
        enemigos.push_back(e);
    }

}

void Level2Widget::keyPressEvent(QKeyEvent *ev)
{
    // 🔥 SI ESTÁ ESPERANDO DECISIÓN (GAME OVER)
    if (esperandoDecision)
    {
        if (ev->key() == Qt::Key_C)   // ✅ CONTINUAR
        {
            reiniciarNivel2();
            return;
        }
        else if (ev->key() == Qt::Key_N)
        {
            emit volverAlMenu();   // vuelve al menú

            return;
        }

        return; // ⛔ BLOQUEA CUALQUIER OTRO INPUT
    }

    // 🔥 BLOQUEAR TECLAS REPETIDAS
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

        if (diff >  totalFrames / 2) diff -= totalFrames;
        if (diff < -totalFrames / 2) diff += totalFrames;

        if (diff > 0)      currentFrame++;
        else if (diff < 0) currentFrame--;

        if (currentFrame < 0) currentFrame += totalFrames;
        if (currentFrame >= totalFrames) currentFrame -= totalFrames;

        updatePlayerSkin();
        animAccumulatorMs = 0;
    }

    // ✅ ACTUALIZAR ENEMIGOS
    for (auto &e : enemigos)
        e.update(dt, width(), height());

    // ✅ DETECTAR COLISIONES
    checkCollisions();

    // 🔥 AQUÍ VA EL GAME OVER (ESTE ERA EL BLOQUE QUE NO SABÍAS DÓNDE IBA)
    if (jugador.vidas <= 0 && !mostrarGameOver)
    {
        mostrarGameOver = true;

        // Detener enemigos
        for (auto &e : enemigos)
            e.activo = false;

        return; // ⛔ detiene el juego aquí
    }

    update(); // ✅ REDIBUJAR
}


void Level2Widget::checkCollisions()
{
    // ❌ Si ya hay Game Over, no revisar más colisiones
    if (mostrarGameOver)
        return;

    for (auto &e : enemigos)
    {
        if (!e.activo || e.enChoque)
            continue;

        if (jugador.getBounds().intersects(e.bounds))
        {
            inter.quitar_vida(1);
            jugador.vidas = inter.contador_vidas;

            // ✅ Activar choque
            e.enChoque = true;
            e.tiempoChoque = 0.0;

            if (!e.spriteChoque.isNull())
                e.sprite = e.spriteChoque;

            // 🔥 GAME OVER
            if (jugador.vidas <= 0)
            {
                mostrarGameOver = true;
                esperandoDecision = true;

                // detener enemigos
                for (auto &en : enemigos)
                    en.activo = false;

                timer.stop();   // ✅ detener el juego
                return;         // ✅ salir inmediatamente
            }
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

    // 🔥 SI HAY GAME OVER, SOLO DIBUJAR ESO
    if (mostrarGameOver)
    {
        if (!gameOverImg.isNull())
            p.drawPixmap(rect(), gameOverImg, gameOverImg.rect());

        // 🔥 TEXTO ABAJO
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 18, QFont::Bold));

        QString texto = "¿Deseas continuar?  (C) Continuar   (N) Volver al menu";
        p.drawText(0, height() - 250, width(), 30,
                   Qt::AlignCenter, texto);

        return;
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



void Level2Widget::reiniciarNivel2()
{
    mostrarGameOver = false;
    esperandoDecision = false;

    inter.contador_vidas = 3;
    jugador.vidas = 3;

    jugador.rect.moveTo(200, 350);

    enemigos.clear();
    enemigosCreados = false;
    setupEnemies();

    timer.start(int(dt * 1000));   // ✅ MUY IMPORTANTE

    update();
}
