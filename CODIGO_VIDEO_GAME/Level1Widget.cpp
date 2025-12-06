#include "Level1Widget.h"
#include <QPainter>
#include <QKeyEvent>
#include <Enemigos.h>
#include <cmath>

Level1Widget::Level1Widget(QWidget *parent)
    : QWidget(parent), dt(0.016)
{
    setFocusPolicy(Qt::StrongFocus);
    // Límites fijos de movimiento en X para el jugador 
    moveXMin = 198;   // borde izquierdo permitido
    moveXMax = 1142;  // borde derecho permitido
    // inicializar jugador en centro abajo
    jugador.rect = QRect( (width()/2) - 25, height() - 140, 50, 80 );
    jugador.vidas = 3;
    jugador.speed = playerMoveSpeed; // usa velocidad configurable

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
            const double scale = 0.35; // hacer el coche más grande
            const int newW = qRound(carPix.width() * scale);
            const int newH = qRound(carPix.height() * scale);
            int margin = playerBottomMargin;
            if (margin < 0) margin = 0; // evitar que valores negativos rompan la lógica
            int y = height() - margin - newH;
            if (y < 0) y = 0;
            if (y > height() - newH) y = height() - newH;
            jugador.rect = QRect(
                (width()/2) - newW/2,
                y,
                newW,
                newH
            );
            // No marcar inicializado aquí: el widget aún puede no tener tamaño real
        }
    }
    // spawn algunos enemigos (lineales desde la izquierda)
    spawnEnemies();

    connect(&timer, &QTimer::timeout, this, &Level1Widget::onTick);
    timer.start(int(dt*1000));
}

void Level1Widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // Mantener al jugador dentro de los límites fijos al redimensionar
    if (jugador.getBounds().left() < moveXMin)
        jugador.rect.moveLeft(moveXMin);
    if (jugador.getBounds().right() > moveXMax)
        jugador.rect.moveRight(moveXMax);

    // Reposicionar vertical según margen inferior ajustable
    int margin = playerBottomMargin;
    if (margin < 0) margin = 0;
    int newY = height() - margin - jugador.rect.height();
    if (newY < 0) newY = 0;
    if (newY > height() - jugador.rect.height()) newY = height() - jugador.rect.height();
    jugador.rect.moveTop(newY);
}

void Level1Widget::spawnEnemies() {
    enemigos.clear();
    for (int i=0;i<5;i++) {
        Enemigos e;
        e.activo = true;
        e.tipo_movimiento = Enemigos::TM_Linear;
        // Tamaño del enemigo y posición inicial dentro del rango [moveXMin, moveXMax]
        e.velocidad = QPointF(0, 150 + i*20);          // velocidad hacia abajo
        e.tamaño = QSize(50,50);
        // Sprite rodador desde Media con rotación alternada
        e.usaSprite = true;
        e.sprite = QPixmap(media.rodador_sprite);
        e.spriteNormal = e.sprite;
        e.rotateSprite = true;
        e.rotationSpeedDeg = 240.0;   // un poco más rápido
        e.rotationToggleInterval = 0.6; // alterna sentido cada ~0.6s
        {
            const int range = std::max(1, moveXMax - moveXMin - e.tamaño.width());
            const int randX = moveXMin + (std::rand() % range);
            e.pos_f = QPointF(randX, -200 - i*150); // aparece arriba dentro del rango
            e.pos_inicial = e.pos_f; // X base para el desplazamiento lateral
            e.pos_base = e.pos_f;    // centro vertical base
            e.tiempo_sen = 0.0;      // temporizador de fase
            e.amplitud_seno = 40.0;  // amplitud lateral (px)
            e.frecuencia_seno = 1.2; // ciclos/seg para parabólico (más rápido)
            e.radio_actual = 36.0;   // amplitud vertical del loop (px) — más grande
            e.velAngular = 5.0;      // velocidad angular del loop (rad/s)
        }
        e.bounds = QRect(int(e.pos_f.x()), int(e.pos_f.y()), e.tamaño.width(), e.tamaño.height());
        enemigos.push_back(e);
    }
}


void Level1Widget::onTick() {
    double sec = dt;
    // Movimiento continuo del jugador basado en teclas mantenidas
    double dx = 0.0;
    const double playerSpeedPxSec = 550.0; // velocidad lateral en px/s
    if (leftHeld)  dx -= playerSpeedPxSec * sec;
    if (rightHeld) dx += playerSpeedPxSec * sec;
    if (dx != 0.0) {
        jugador.moverPorVector(float(dx), 0.0f);
        // Clamp a límites fijos
        if (jugador.getBounds().left() < moveXMin)
            jugador.rect.moveLeft(moveXMin);
        if (jugador.getBounds().right() > moveXMax)
            jugador.rect.moveRight(moveXMax);
    }
    // actualizar enemigos (lineales)
    for (auto &e : enemigos) {
        if (!e.activo) continue;
        if (e.tipo_movimiento == Enemigos::TM_Linear) {
            // Centro vertical base: bajar recto
            e.pos_base.setY(e.pos_base.y() + e.velocidad.y() * sec);

            // Desplazamiento lateral parabólico por ciclos
            // Definición: offsetX = dir * A * (1 - u^2), donde u = 2*phase - 1 en [−1,1]
            // - A: amplitud (px), dir alterna izquierda/derecha por ciclo
            // - phase: t/T con T = 1/frecuencia
            e.tiempo_sen += sec; // acumulador de tiempo
            double A = e.amplitud_seno; // amplitud lateral
            double f = (e.frecuencia_seno <= 0.0) ? 1.0 : e.frecuencia_seno; // ciclos/seg
            double T = 1.0 / f; // periodo en segundos
            double t = e.tiempo_sen;
            int cycles = int(t / T);
            int dir = (cycles % 2 == 0) ? -1 : 1; // alterna cada ciclo
            double phase = std::fmod(t, T) / T; // [0,1)
            double u = 2.0 * phase - 1.0; // [-1,1]
            double offsetX = dir * A * (1.0 - (u*u)); // parábola invertida, 0 en los extremos, pico en el centro

            double baseX = e.pos_inicial.x();
            double newX = baseX + offsetX;
            // Clamp al rango permitido
            int leftBound = moveXMin;
            int rightBound = moveXMax - e.tamaño.width();
            if (newX < leftBound) newX = leftBound;
            if (newX > rightBound) newX = rightBound;
            e.pos_f.setX(newX);

            // Desplazamiento vertical tipo loop (sinusoide) alrededor del centro que baja
            double By = (e.radio_actual <= 0.0) ? 20.0 : e.radio_actual;      // amplitud vertical (px)
            double omega = (e.velAngular <= 0.0) ? 5.0 : e.velAngular;        // rad/s
            double yLoop = By * std::sin(omega * t);
            e.pos_f.setY(e.pos_base.y() + yLoop);

            // Respawn arriba si sale por abajo
            if (e.pos_base.y() > height() + 50) {
                e.pos_base.setY(-100);              // reaparece arriba (centro)
                const int range = std::max(1, moveXMax - moveXMin - e.tamaño.width());
                e.pos_f.setX(moveXMin + (std::rand() % range));  // posición aleatoria dentro del rango
                // Reiniciar base y fase para el siguiente ciclo parabólico
                e.pos_inicial = e.pos_f;
                e.tiempo_sen = 0.0;
                e.pos_f.setY(e.pos_base.y());
            }

            e.bounds.moveTopLeft(QPoint(int(e.pos_f.x()), int(e.pos_f.y())));
            // Asegurar que el rodador rote aunque no use Enemigos::update
            e.stepRotation(sec);
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
            // respawn enemigo arriba dentro del rango permitido
            e.pos_f.setY(-150);
            const int range = std::max(1, moveXMax - moveXMin - e.tamaño.width());
            e.pos_f.setX(moveXMin + (std::rand() % range));
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

    // Posicionar al jugador con margen inferior la primera vez si no estaba inicializado
    if (!playerPosInitialized) {
        const int newW = jugador.rect.width();
        const int newH = jugador.rect.height();
        int margin = playerBottomMargin;
        if (margin < 0) margin = 0;
        int y = height() - margin - newH;
        if (y < 0) y = 0;
        if (y > height() - newH) y = height() - newH;
        jugador.rect.moveTo((width()/2) - newW/2, y);
        playerPosInitialized = true;
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
        leftHeld = true;
        // movimiento inmediato para eliminar sensación de retraso inicial
        jugador.moverIzquierda();
        if (jugador.getBounds().left() < moveXMin) jugador.rect.moveLeft(moveXMin);
    } else if (key == Qt::Key_D || key == Qt::Key_Right) {
        rightHeld = true;
        // movimiento inmediato para eliminar sensación de retraso inicial
        jugador.moverDerecha();
        if (jugador.getBounds().right() > moveXMax) jugador.rect.moveRight(moveXMax);
    }
    update();
}

void Level1Widget::keyReleaseEvent(QKeyEvent *event) {
    const int key = event->key();
    if (key == Qt::Key_A || key == Qt::Key_Left) {
        leftHeld = false;
    } else if (key == Qt::Key_D || key == Qt::Key_Right) {
        rightHeld = false;
    }
    update();
}
