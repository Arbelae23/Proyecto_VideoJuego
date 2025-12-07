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
    // Cargar imagen de fondo de Nivel 3 desde Media (caché dinámico)
    try {
        background = media.getBackgroundNivel3();
    } catch (const MediaLoadError &ex) {
        qDebug() << "Level3: fallo al cargar fondo:" << ex.what();
        background = QPixmap();
    }

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

    // No crear trofeo inmediatamente: esperar un pequeño retraso inicial
    trophyRespawnSec = initialTrophyDelaySec;

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
        e.velocidadOriginal = e.velocidad; // conservar velocidad para recuperación tras choque
        // Tamaño inicial mínimo (consistente con minH=24 en onTick)
        e.tamaño = QSize(40, 24);
        // Asignar sprite aleatorio entre 3 opciones con excepciones
        e.usaSprite = true;
        try {
            switch (std::rand() % 3) {
                case 0: e.sprite = media.getCarro1(); break;
                case 1: e.sprite = media.getCarro2(); break;
                default: e.sprite = media.getCarro3(); break;
            }
        } catch (const MediaLoadError &ex) {
            qDebug() << "Level3: fallo sprite auto (spawn):" << ex.what();
            e.sprite = QPixmap();
        }
        e.spriteNormal = e.sprite;
        // Sprite de choque para explosión visual
        try {
            e.spriteChoque = media.getChoque();
        } catch (const MediaLoadError &ex) {
            qDebug() << "Level3: fallo sprite choque (spawn):" << ex.what();
        }
        e.bounds = QRect(int(e.pos_f.x()), int(e.pos_f.y()), e.tamaño.width(), e.tamaño.height());
        enemigos.push_back(e);
    }
}

// Utilidad local para obtener un aleatorio [0,1] sin depender de QRandomGenerator
static inline double rand01() {
    return double(std::rand()) / double(RAND_MAX);
}

void Level3Widget::spawnTrophy() {
    // Permitir solo un trofeo activo a la vez
    trofeos.clear();

    // Elegir carril con mayor distancia vertical respecto a los autos en spawn,
    // evitando repetir el último carril usado
    const int laneXs[3] = { laneX1, laneX2, laneX3 };
    double laneScore[3] = { -1.0, -1.0, -1.0 };
    for (int li=0; li<3; ++li) {
        double minDist = 1e9;
        for (auto &e : enemigos) {
            if (!e.activo) continue;
            if (e.lane == li) {
                minDist = std::min(minDist, std::abs(e.pos_f.y() - double(laneSpawnY)));
            }
        }
        laneScore[li] = minDist;
    }
    // Construir lista de candidatos ordenados por distancia descendente
    std::array<int,3> lanes = {0,1,2};
    std::sort(lanes.begin(), lanes.end(), [&](int a, int b){ return laneScore[a] > laneScore[b]; });
    int bestLane = lanes[0];
    // Evitar carril repetido si hay alternativas razonables
    if (bestLane == lastTrophyLane) {
        for (int i=1; i<3; ++i) {
            if (lanes[i] != lastTrophyLane) { bestLane = lanes[i]; break; }
        }
    }
    double bestDist = laneScore[bestLane];
    // Si todos los carriles están ocupados cerca, usar retraso adicional aleatorio
    if (bestDist < 120.0) {
        double jitter = 0.6 + rand01() * 0.8; // 0.6–1.4s
        trophyRespawnSec = jitter;
        return;
    }

    Enemigos t;
    t.activo = true;
    t.tipo_objeto = "trofeo";
    t.tipo_movimiento = Enemigos::TM_Linear;
    t.lane = bestLane;
    int spawnX = laneXs[bestLane];
    t.pos_f = QPointF(spawnX, laneSpawnY);
    t.pos_inicial = t.pos_f;
    t.velocidad = QPointF(0, 150);
    t.tamaño = QSize(40, 24);
    t.usaSprite = true;
    try {
        t.sprite = media.getTrofeo();
    } catch (const MediaLoadError &ex) {
        qDebug() << "Level3: fallo sprite trofeo:" << ex.what();
        t.sprite = QPixmap();
    }
    t.spriteNormal = t.sprite;
    t.bounds = QRect(int(t.pos_f.x()), int(t.pos_f.y()), t.tamaño.width(), t.tamaño.height());
    trofeos.push_back(t);
    lastTrophyLane = bestLane;
}

void Level3Widget::onTick() {
    double sec = dt;
    // Temporizador en segundo plano (no visible)
    if (!nivelGanado) {
        tiempoAcumulado += sec;
    }

    // Gestionar respawn diferido de trofeos
    bool trophyActive = false;
    for (auto &t : trofeos) if (t.activo) { trophyActive = true; break; }
    if (!trophyActive) {
        if (trophyRespawnSec > 0.0) {
            trophyRespawnSec -= sec;
        }
        if (trophyRespawnSec <= 0.0) {
            spawnTrophy();
            // resetear para evitar spawns múltiples en el mismo tick
            trophyRespawnSec = 0.0;
        }
    }
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
        // Descontar cooldown manualmente para re-habilitar daño tras explosión
        if (e.tiempoCooldown > 0.0) {
            e.tiempoCooldown -= sec;
            if (e.tiempoCooldown < 0.0) e.tiempoCooldown = 0.0;
            e.yaGolpeo = true;
        } else {
            e.yaGolpeo = false;
        }
        // Si está en estado de choque, dejar que el enemigo gestione su animación y pausa
        if (e.enChoque) {
            e.update(sec, width(), height());
            continue;
        }
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
                // Cambiar sprite aleatoriamente en cada respawn (opcional) con excepciones
                try {
                    switch (std::rand() % 3) {
                        case 0: e.sprite = media.getCarro1(); break;
                        case 1: e.sprite = media.getCarro2(); break;
                        default: e.sprite = media.getCarro3(); break;
                    }
                } catch (const MediaLoadError &ex) {
                    qDebug() << "Level3: fallo sprite auto (respawn):" << ex.what();
                    e.sprite = QPixmap();
                }
                e.spriteNormal = e.sprite;
            }
            e.bounds.moveTopLeft(QPoint(int(e.pos_f.x()), int(e.pos_f.y())));
        }
    }

    // Actualizar trofeo con misma lógica de carriles y crecimiento
    for (auto &t : trofeos) {
        if (!t.activo) continue;
        if (t.tipo_movimiento == Enemigos::TM_Linear) {
            // Movimiento vertical principal
            t.pos_f.setY(t.pos_f.y() + t.velocidad.y() * sec);

            // Deriva lateral según carril, dentro de la carretera
            const double lateralSpeed = 220.0;
            int leftBoundX = roadX;
            int rightBoundX = roadX + roadWidth - t.tamaño.width();
            if (t.lane == 0) {
                t.pos_f.setX(std::max<double>(leftBoundX, t.pos_f.x() - lateralSpeed * sec));
            } else if (t.lane == 2) {
                t.pos_f.setX(std::min<double>(rightBoundX, t.pos_f.x() + lateralSpeed * sec));
            }

            // Escalado por perspectiva
            const double minH = 15.0;
            const double maxH = 330.0;
            const double aspect = 60.0/36.0;
            double range = (height() - t.pos_inicial.y()) + 120.0;
            if (range < 1.0) range = 1.0;
            double norm = (t.pos_f.y() - t.pos_inicial.y()) / range;
            norm = std::clamp(norm, 0.0, 1.0);
            double curve = std::pow(norm, 1.4);
            int newH = int(minH + curve * (maxH - minH));
            int newW = int(newH * aspect);
            t.tamaño = QSize(newW, newH);
            t.bounds.setSize(t.tamaño);
            // Si sale por abajo y no fue atrapado: desactivar y reprogramar respawn en otro carril
            if (t.pos_f.y() > height() + 80) {
                lastTrophyLane = t.lane; // recordar carril actual para no repetir
                t.activo = false;
                double jitter = 0.8 + rand01() * 0.9; // 0.8–1.7s
                trophyRespawnSec = jitter;
            }
            t.bounds.moveTopLeft(QPoint(int(t.pos_f.x()), int(t.pos_f.y())));
        }
    }
    checkCollisions();

    // Condición de victoria: tiempo >= 35s y trofeos completos
    if (!nivelGanado && tiempoAcumulado >= 35.0 && trofeosRecolectados >= totalTrofeosObjetivo) {
        nivelGanado = true;
        // Detener movimiento general
        for (auto &e : enemigos) e.activo = false;
        for (auto &t : trofeos) t.activo = false;
        timer.stop();
        mostrarVictoria = true;
        esperandoDecision = true;
    }

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
        // Evitar daño repetido: respetar estado de choque y cooldown del enemigo
        if (e.enChoque || e.tiempoCooldown > 0.0) continue;

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
            // Activar explosión del enemigo tal como en Nivel 2
            e.yaGolpeo = true;   // marcar que ya golpeó en este ciclo
            e.enChoque = true;
            e.tiempoChoque = 0.0;
            e.velocidad = QPointF(0,0);
            if (!e.spriteChoque.isNull())
                e.sprite = e.spriteChoque;
            // Evitar múltiples daños en el mismo tick por colisiones simultáneas
            break;
        }
    }
    jugador.vidas = inter.contador_vidas;

    // Colisión con trofeos: aplicar mismas reglas que enemigos (hitbox recortado, área mínima y tamaño similar)
    for (auto &t : trofeos) {
        if (!t.activo) continue;
        // Hitbox del jugador con recorte adicional
        QRect playerHB = jugador.getHitbox();
        int extraX = int(playerHB.width() * l3ExtraInsetXRatio);
        int extraY = int(playerHB.height() * l3ExtraInsetYRatio);
        if (extraX > 0 || extraY > 0) playerHB = playerHB.adjusted(extraX, extraY, -extraX, -extraY);

        QRect trophyB = t.getBounds();
        QRect overlap = playerHB.intersected(trophyB);
        // Exigir área mínima de solapamiento
        double minArea = l3MinOverlapAreaRatio * (trophyB.width() * trophyB.height());
        if (overlap.isNull() || (overlap.width() * overlap.height()) < minArea) continue;

        // Verificación de "profundidad" similar a enemigos: tamaño esperado alrededor de 140px de alto
        const int expectedH = 140;
        const int tolerancePx = 20;
        bool similarScale = std::abs(t.getBounds().height() - expectedH) <= tolerancePx;
        if (!similarScale) continue;

        // Recoger trofeo: efecto de crecimiento breve y conteo
        t.tamaño = QSize(int(t.tamaño.width() * 1.3), int(t.tamaño.height() * 1.3));
        t.bounds.setSize(t.tamaño);
        media.reproducir_sonidoTrofeo();
        if (trofeosRecolectados < totalTrofeosObjetivo)
            trofeosRecolectados++;
        // Desaparecer y programar respawn en 3 segundos
        t.activo = false;
        trophyRespawnSec = 3.0;
        break; // sólo uno por tick
    }

    // Game Over si se quedan sin vidas
    if (jugador.vidas <= 0 && !mostrarGameOver) {
        mostrarGameOver = true;
        esperandoDecision = true;
        for (auto &e : enemigos) e.activo = false;
        for (auto &t : trofeos) t.activo = false;
        timer.stop();
    }
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
    if (esperandoDecision) {
        if (key == Qt::Key_C) {
            reiniciarNivel3();
            return;
        } else if (key == Qt::Key_N) {
            emit volverAlMenu();
            return;
        }
    }
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

    // Dibujar trofeo con mismo estilo
    for (auto &t : trofeos) {
        if (t.activo) t.draw(p);
    }

    // Dibujar jugador por encima de los enemigos y trofeos
    jugador.draw(p);

    // HUD: mostrar vidas arriba y trofeos debajo (sin tiempo en pantalla)
    QFont hudFont("Arial", 28, QFont::Bold);
    p.setFont(hudFont);
    p.setPen(Qt::white);
    p.drawText(QPoint(10, 40), QString("Vidas: %1").arg(jugador.vidas));
    p.drawText(QPoint(10, 90), QString("Trofeos: %1/%2").arg(trofeosRecolectados).arg(totalTrofeosObjetivo));

    // Overlays Game Over / Victoria estilo niveles 1 y 2
    if (mostrarGameOver || mostrarVictoria) {
        p.fillRect(rect(), QColor(0,0,0,160));
        QPixmap overlayImg;
        try {
            overlayImg = mostrarGameOver ? media.getGameOver() : media.getVictoria();
        } catch (const MediaLoadError &ex) {
            qDebug() << "Level3: fallo overlay:" << ex.what();
            overlayImg = QPixmap();
        }
        if (!overlayImg.isNull()) {
            QSize targetSize(int(width()*0.7), int(height()*0.7));
            QPixmap scaled = overlayImg.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            int cx = (width() - scaled.width())/2;
            int cy = (height() - scaled.height())/2;
            p.drawPixmap(cx, cy, scaled);
        }
        QFont f("Arial", 24, QFont::Bold);
        p.setFont(f);
        p.setPen(Qt::white);
        QRect instrRect(0, height()-180, width(), 100);
        p.drawText(instrRect, Qt::AlignCenter, "Presiona C para reintentar o N para volver al menú");
    }
}

void Level3Widget::reiniciarNivel3() {
    // Reset flags
    mostrarGameOver = false;
    mostrarVictoria = false;
    esperandoDecision = false;
    nivelGanado = false;
    tiempoAcumulado = 0.0;
    trofeosRecolectados = 0;

    // Reset vidas
    inter.contador_vidas = 3;
    jugador.vidas = inter.contador_vidas;

    // Reposicionar jugador
    playerPosInitialized = false;

    // Respawn enemigos y limpiar trofeo
    spawnObstacles();
    trofeos.clear();
    // Aleatorizar retraso inicial y evitar repetir carril previo
    lastTrophyLane = -1;
    initialTrophyDelaySec = 1.0 + rand01() * 1.2; // 1.0–2.2s
    trophyRespawnSec = initialTrophyDelaySec; // retraso inicial para evitar simultaneidad con autos

    // Reset input
    leftHeld = rightHeld = false;

    // Reanudar loop
    if (!timer.isActive()) timer.start(int(dt*1000));
    update();
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
