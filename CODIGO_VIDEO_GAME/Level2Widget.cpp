#include "Level2Widget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QtMath>
#include <cstdlib>
#include <QDebug>

Level2Widget::Level2Widget(QWidget *parent)
    : QWidget(parent), dt(0.016), t_global(0)
{

    timerTrofeos = new QTimer(this);
    connect(timerTrofeos, &QTimer::timeout, this, &Level2Widget::spawnTrofeo);
    timerTrofeos->start(5000);   // Cada 5 segundos


    mostrarGameOver = false;
    esperandoDecision = false;
    enemigosCreados = false;

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // cargar media primero (IMPORTANTE)
    media.cargarMedia();

    // --- Inicializar jugador ---
    jugador.rect = QRect(200, 350, 50, 80);
    jugador.speed = 8;
    jugador.vidas = 3;

    // cargar background desde Media usando caché dinámico
    try {
        background = media.getBackgroundNivel2();
    } catch (const MediaLoadError &ex) {
        qDebug() << "Level2: fallo al cargar fondo:" << ex.what();
        background = QPixmap();
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

        // ESPACIADO REAL EN TODO EL ANCHO
        int separacionX = width() / 3;
        e.pos_base.setX(i * separacionX + separacionX /2);

        // ESPACIADO VERTICAL CONTROLADO
        e.pos_base.setY(50 + i * 100);

        // VELOCIDADES DISTINTAS
        e.velocidad = QPointF(0, 100 + i * 40 + rand() % 30);

        e.velocidadOriginal = e.velocidad;

        // SENO DISTINTO PARA CADA UNO
        e.amplitud_seno = 50 + i * 15;
        e.frecuencia_seno = 1.5 + (i * 0.4);

        e.bounds = QRect(
            int(e.pos_base.x()),
            int(e.pos_base.y()),
            e.tamaño.width(),
            e.tamaño.height()
            );

        // SPRITES POLICÍA — 8 DIRECCIONES (caché dinámico)
        try {
            e.spriteNormalArriba    = media.getPoliciaW();
            e.spriteNormalAbajo     = media.getPoliciaS();
            e.spriteNormalIzquierda = media.getPoliciaA();
            e.spriteNormalDerecha   = media.getPoliciaD();
            e.spriteDiagonalSD      = media.getPoliciaSD();
            e.spriteDiagonalSA      = media.getPoliciaSA();
            e.spriteDiagonalWD      = media.getPoliciaWD();
            e.spriteDiagonalWA      = media.getPoliciaWA();
        } catch (const MediaLoadError &ex) {
            qDebug() << "Level2: fallo sprites policía:" << ex.what();
        }

        // Estado inicial: mirando abajo
        if (!e.spriteNormalAbajo.isNull()) {
            e.sprite = e.spriteNormalAbajo;
            e.spriteNormal = e.spriteNormalAbajo;
            e.mirandoArriba = false;
            e.usaSprite = true;
        }


        // Sprite de choque (caché dinámico)
        try {
            e.spriteChoque = media.getChoque();
        } catch (const MediaLoadError &ex) {
            qDebug() << "Level2: fallo sprite choque:" << ex.what();
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

        //  SEPARACIÓN HORIZONTAL MÁS AMPLIA
        int separacionX = 1000;   // antes 150 (muy juntos)
        e.pos_base.setX(-150 - i * separacionX);

        //  SEPARACIÓN VERTICAL CONTROLADA
        e.pos_base.setY(140 + i * 300);

        //  VELOCIDAD DIFERENTE PARA CADA UNO
        int velBase = 140;
        int variacion = rand() % 80;   // velocidades distintas
        e.velocidad = QPointF(velBase + variacion, 0);

        //  PARÁMETROS DISTINTOS DE ESPIRAL
        e.radio_actual = 30 + rand() % 40;
        e.velAngular = 4 + rand() % 4;

        e.bounds = QRect(
            int(e.pos_base.x()),
            int(e.pos_base.y()),
            e.tamaño.width(),
            e.tamaño.height()
            );

        //  Sprite de la bicicleta (caché dinámico)
        try {
            e.spriteNormal = media.getBicicleta();
            e.spriteNormalIzquierda = e.spriteNormal.transformed(
                QTransform().scale(-1, 1));  // espejo
            e.sprite = e.spriteNormal;
            e.mirandoDerecha = true;
            e.usaSprite = true;
        } catch (const MediaLoadError &ex) {
            qDebug() << "Level2: fallo sprite bicicleta:" << ex.what();
        }

        try {
            e.spriteChoque = media.getChoque();
        } catch (const MediaLoadError &ex) {
            qDebug() << "Level2: fallo sprite choque (bici):" << ex.what();
        }

        e.pos_inicial = e.pos_base;
        e.pos_f = e.pos_base;

        e.velocidadOriginal = e.velocidad;
        enemigos.push_back(e);
    }



    //  ENEMIGO INTELIGENTE
    Enemigos bot;
    bot.activo = true;
    bot.tipo_movimiento = Enemigos::TM_Inteligente;

    bot.tamaño = QSize(100, 80);
    bot.pos_base = QPointF(width()/2, height()/2);
    bot.pos_f = bot.pos_base;
    bot.velocidad = QPointF(140, 0);
    bot.velocidadOriginal = bot.velocidad;
    bot.radioVision = 250;
    bot.radioPerdida = 320;

    bot.bounds = QRect(
        int(bot.pos_f.x()),
        int(bot.pos_f.y()),
        bot.tamaño.width(),
        bot.tamaño.height()
        );

    // sprite
    if (!media.IA.isEmpty())   // SPRITE "IA"
    {
        bot.spriteNormal.load(media.IA);
        bot.sprite = bot.spriteNormal;
        bot.usaSprite = true;     // OBLIGATORIO PARA EVITAR EL CÍRCULO ROJO
    }

    if (!media.Choque.isEmpty())
    {
        bot.spriteChoque.load(media.Choque);
    }




    enemigos.push_back(bot);

}

void Level2Widget::keyPressEvent(QKeyEvent *ev)
{
    //  SI ESTÁ ESPERANDO DECISIÓN (GAME OVER)
    if (esperandoDecision)
    {
        if (ev->key() == Qt::Key_C)   //  CONTINUAR
        {
            reiniciarNivel2();
            return;
        }
        else if (ev->key() == Qt::Key_N)
        {
            emit volverAlMenu();   // vuelve al menú

            return;
        }

        return; //  BLOQUEA CUALQUIER OTRO INPUT
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

    // ---- MOVIMIENTO NORMALIZADO CON CLAMP (sin "teletransporte") ----
    if (dx != 0 || dy != 0)
    {
        float len = std::sqrt(dx * dx + dy * dy);
        dx = (dx / len) * jugador.speed;
        dy = (dy / len) * jugador.speed;

        // Calcular nueva posición y limitar a los bordes
        int minX = 0;
        int maxX = width() - jugador.rect.width();
        int minY = 0;
        int maxY = height() - jugador.rect.height();

        int newX = int(std::round(jugador.rect.x() + dx));
        int newY = int(std::round(jugador.rect.y() + dy));

        if (newX < minX) newX = minX;
        if (newX > maxX) newX = maxX;
        if (newY < minY) newY = minY;
        if (newY > maxY) newY = maxY;

        jugador.rect.moveTo(newX, newY);
    }
}

void Level2Widget::onTick()
{



    // TIEMPO
    tiempoRestante -= dt;

    if (tiempoRestante <= 0 && !nivelGanado)
    {
        mostrarGameOver = true;
        esperandoDecision = true;

        for (auto &e : enemigos)
            e.activo = false;

        timer.stop();
        update();   //  fuerza el dibujado del Game Over
        return;
    }


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


    for (auto &e : enemigos)
    {
        if (e.tipo_movimiento == Enemigos::TM_Inteligente)
            e.objetivo = jugador.rect.center();
    }

    //  ACTUALIZAR ENEMIGOS
    for (auto &e : enemigos)
        e.update(dt, width(), height());

    for (auto &t : trofeos)
        t.update(dt);

    //  DETECTAR COLISIONES
    checkCollisions();

    //  AQUÍ VA EL GAME OVER (ESTE ERA EL BLOQUE QUE NO SABÍAS DÓNDE IBA)
    if (jugador.vidas <= 0 && !mostrarGameOver)
    {
        mostrarGameOver = true;

        // Detener enemigos
        for (auto &e : enemigos)
            e.activo = false;

        return; //  detiene el juego aquí
    }

    update(); //  REDIBUJAR
}


void Level2Widget::checkCollisions()
{
    // Si ya hay Game Over, no revisar más colisiones
    if (mostrarGameOver)
        return;

    for (auto &e : enemigos)
    {
        if (!e.activo || e.enChoque || e.tiempoCooldown > 0.0)
            continue;

        if (jugador.getBounds().intersects(e.bounds))
        {
            e.yaGolpeo = true;   //  ya causó daño

            inter.quitar_vida(1);
            jugador.vidas = inter.contador_vidas;

            //  Activar choque
            e.enChoque = true;
            e.tiempoChoque = 0.0;
            e.velocidad = QPointF(0,0);

            if (!e.spriteChoque.isNull())
                e.sprite = e.spriteChoque;

            //  GAME OVER
            if (jugador.vidas <= 0)
            {
                mostrarGameOver = true;
                esperandoDecision = true;

                // detener enemigos
                for (auto &en : enemigos)
                    en.activo = false;

                timer.stop();   //  detener el juego
                return;         //  salir inmediatamente
            }
        }
    }

    // COLISIÓN CON TROFEOS
    for (auto &t : trofeos)
    {
        if (t.activo && jugador.getBounds().intersects(t.bounds))
        {
            t.desaparecer();
            trofeosRecolectados++;   //  CONTAR
        }
    }

    if (trofeosRecolectados >= totalTrofeosObjetivo)
    {
        nivelGanado = true;
        mostrarVictoria = true;     //  ahora sí muestra victoria
        esperandoDecision = true;

        // detener enemigos
        for (auto &en : enemigos)
            en.activo = false;

        timer.stop();
        update();
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


    //  SI GANÓ, MOSTRAR PANTALLA DE VICTORIA (estilo Nivel 1)
    if (mostrarVictoria)
    {
        // Fondo semi-transparente
        p.fillRect(rect(), QColor(0,0,0,128));
        // Imagen centrada y escalada (desde caché Media)
        QPixmap overlay = media.getVictoria();
        QSize targetSize = overlay.size();
        const int maxW = width() * 0.7;
        const int maxH = height() * 0.7;
        if (targetSize.width() > maxW || targetSize.height() > maxH) {
            overlay = overlay.scaled(maxW, maxH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            targetSize = overlay.size();
        }
        QPoint topLeft((width() - targetSize.width())/2, (height() - targetSize.height())/2 - 40);
        p.drawPixmap(topLeft, overlay);

        // Texto de instrucciones (C/N)
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 18, QFont::Bold));
        p.drawText(QRect(0, height()-180, width(), 100), Qt::AlignCenter,
                   QString::fromUtf8("Presiona C para reintentar o N para volver al menú"));

        return;   // NO DIBUJAR NADA MÁS
    }







    //  SI HAY GAME OVER, SOLO DIBUJAR ESO (estilo Nivel 1)
    if (mostrarGameOver)
    {
        // Fondo semi-transparente
        p.fillRect(rect(), QColor(0,0,0,128));
        // Imagen centrada y escalada (desde caché Media)
        QPixmap overlay = media.getGameOver();
        QSize targetSize = overlay.size();
        const int maxW = width() * 0.7;
        const int maxH = height() * 0.7;
        if (targetSize.width() > maxW || targetSize.height() > maxH) {
            overlay = overlay.scaled(maxW, maxH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            targetSize = overlay.size();
        }
        QPoint topLeft((width() - targetSize.width())/2, (height() - targetSize.height())/2 - 40);
        p.drawPixmap(topLeft, overlay);

        // Texto de instrucciones (C/N)
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 18, QFont::Bold));
        p.drawText(QRect(0, height()-180, width(), 100), Qt::AlignCenter,
                   QString::fromUtf8("Presiona C para reintentar o N para volver al menú"));

        return;   //  IMPORTANTE: no dibuja nada más
    }

    //  DIBUJAR TROFEOS (ANTES DE ENEMIGOS Y JUGADOR)
    for (auto &t : trofeos)
        t.draw(p);

    // (Eliminado ajuste de posición automático para evitar saltos indeseados)

    //  DIBUJAR ENEMIGOS
    for (auto &e : enemigos) {
        e.draw(p);
    }

    // JUGADOR ENCIMA DE LOS ENEMIGOS
    jugador.draw(p);

    // HUD grande y blanco (como Nivel 1)
    {
        QFont f = p.font();
        f.setBold(true);
        f.setPointSize(28);
        p.setFont(f);
        p.setPen(Qt::white);
        p.drawText(10, 40, QString("Vidas: %1").arg(jugador.vidas));
        p.drawText(10, 90, QString("Tiempo: %1").arg((int)tiempoRestante));
        p.drawText(10, 140, QString("Trofeos: %1 / %2")
                                .arg(trofeosRecolectados)
                                .arg(totalTrofeosObjetivo));
    }

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
    mostrarVictoria = false;
    mostrarGameOver = false;
    esperandoDecision = false;
    nivelGanado = false;

    inter.contador_vidas = 3;
    jugador.vidas = 3;

    jugador.rect.moveTo(200, 350);

    enemigos.clear();
    enemigosCreados = false;
    setupEnemies();

    trofeos.clear();
    trofeosRecolectados = 0;

    tiempoRestante = tiempoLimite;

    timer.start(int(dt * 1000));
    update();
}



void Level2Widget::spawnTrofeo()
{
    if (mostrarGameOver || nivelGanado) return;

    Objeto_Interactivo t;

    // Tamaño fijo del trofeo y márgenes seguros para evitar bordes
    const int trofeoW = 80;
    const int trofeoH = 80;
    const int margin = 50; // margen visual respecto a límites

    int minX = margin;
    int maxX = width()  - trofeoW - margin;
    int minY = margin;
    int maxY = height() - trofeoH - margin;

    // Salvaguardas por si la ventana es pequeña
    if (maxX < minX) { minX = 0; maxX = std::max(0, width()  - trofeoW); }
    if (maxY < minY) { minY = 0; maxY = std::max(0, height() - trofeoH); }

    int x = minX;
    int y = minY;
    if (maxX > minX) x = minX + (std::rand() % (maxX - minX + 1));
    if (maxY > minY) y = minY + (std::rand() % (maxY - minY + 1));

    t.tamaño = QSize(trofeoW, trofeoH);
    t.aparecer(QPoint(x, y));
    t.bounds = QRect(t.spawn_xy, t.tamaño);
    t.tiempo_pantalla = 5.0;

    // ASIGNAR SPRITE DEL TROFEO
    if (!media.trofeo_sprite.isEmpty())
        t.sprite.load(media.trofeo_sprite);

    trofeos.push_back(t);
}


