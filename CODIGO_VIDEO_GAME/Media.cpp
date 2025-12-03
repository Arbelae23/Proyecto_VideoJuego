#include "Media.h"
#include <QDebug>
#include <cmath>

Media::Media() {
    cargarMedia();
}

void Media::cargarMedia() {
    // Aquí asignas rutas o inicializas QMediaPlayer si quieres
    cancion_nivel = "";
    // Establecer fondo por defecto para el nivel actual (Nivel 1)
    // Usar recurso embebido para evitar problemas de rutas relativas
    background_nivel = "C:/Users/USER/Documents/GitHub/Proyecto_VideoJuego/CODIGO_VIDEO_GAME/Backgraund/fon1.jpg";
    camino = "C:/Users/USER/Documents/GitHub/Proyecto_VideoJuego/CODIGO_VIDEO_GAME/Backgraund/camino.png";

    background_nivel2 ="C:/Users/USER/Documents/GitHub/Proyecto_VideoJuego/CODIGO_VIDEO_GAME/Backgraund/fon2.png";

    background_nivel3 ="C:/Users/USER/Documents/GitHub/Proyecto_VideoJuego/CODIGO_VIDEO_GAME/Backgraund/fon3.png";

    jugador1_sprite = "C:/Users/USER/Documents/GitHub/Proyecto_VideoJuego/CODIGO_VIDEO_GAME/sprite/Car1.png";

    jugador3_sprite_2 = "C:/Users/USER/Documents/GitHub/Proyecto_VideoJuego/CODIGO_VIDEO_GAME/sprite/-2m.png";
    jugador3_sprite_1 = "C:/Users/USER/Documents/GitHub/Proyecto_VideoJuego/CODIGO_VIDEO_GAME/sprite/-1m.png";
    jugador3_sprite0 = "C:/Users/USER/Documents/GitHub/Proyecto_VideoJuego/CODIGO_VIDEO_GAME/sprite/0m.png";
    jugador3_sprite1 = "C:/Users/USER/Documents/GitHub/Proyecto_VideoJuego/CODIGO_VIDEO_GAME/sprite/1m.png";
    jugador3_sprite2 = "C:/Users/USER/Documents/GitHub/Proyecto_VideoJuego/CODIGO_VIDEO_GAME/sprite/2m.png";

    sonido_arresto = "";
    sonido_choque = "";
    sonido_trofeo = "";
    sonido_win = "";
    sonido_loss = "";
}

void Media::reproducir_cancionNivel(int nivel) {
    Q_UNUSED(nivel);
    qDebug() << "Play level music (stub)";
}

void Media::reproducir_background() { qDebug() << "Play background (stub)"; }
void Media::reproducir_sonidoChoque() { qDebug() << "Play crash (stub)"; }
void Media::reproducir_sonidoTrofeo() { qDebug() << "Play trophy (stub)"; }
void Media::reproducir_win() { qDebug() << "Play win (stub)"; }
void Media::reproducir_loss() { qDebug() << "Play loss (stub)"; }

void Media::setRoadSpeed(int pxPerSec) {
    caminoSpeedPxPerSec = pxPerSec;
}

void Media::drawScrollingRoad(QPainter &p, const QRect &area, double dt) {
    // Cargar una vez el pixmap base
    if (caminoPixmap.isNull() && !camino.isEmpty()) {
        caminoPixmap.load(camino);
    }
    if (caminoPixmap.isNull()) return;

    // Asegurar una versión pre-escalada al ancho del área (una sola copia horizontal)
    if (caminoScaled.isNull() || caminoScaledWidth != area.width()) {
        caminoScaled = caminoPixmap.scaledToWidth(area.width(), Qt::SmoothTransformation);
        caminoScaledWidth = caminoScaled.width();
        caminoScaledHeight = caminoScaled.height();
        // Reiniciar offset si cambió la escala para evitar saltos
        caminoOffsetY = fmod(caminoOffsetY, double(caminoScaledHeight));
    }

    // Actualizar offset vertical en px de la imagen escalada
    caminoOffsetY += caminoDirection * (caminoSpeedPxPerSec * dt);
    if (caminoOffsetY >= caminoScaledHeight || caminoOffsetY <= -caminoScaledHeight) {
        caminoOffsetY = std::fmod(caminoOffsetY, double(caminoScaledHeight));
    }
    if (caminoOffsetY < 0) caminoOffsetY += caminoScaledHeight; // mantener en [0, H)

    // Dibujar en dos segmentos verticales para scroll fluido
    int offset = int(caminoOffsetY);
    int firstSegH = std::min(area.height(), caminoScaledHeight - offset);
    if (firstSegH > 0) {
        QRect targetTop(area.x(), area.y(), area.width(), firstSegH);
        QRect sourceTop(0, offset, caminoScaledWidth, firstSegH);
        p.drawPixmap(targetTop, caminoScaled, sourceTop);
    }
    int remainingH = area.height() - firstSegH;
    if (remainingH > 0) {
        QRect targetBottom(area.x(), area.y() + firstSegH, area.width(), remainingH);
        QRect sourceBottom(0, 0, caminoScaledWidth, remainingH);
        p.drawPixmap(targetBottom, caminoScaled, sourceBottom);
    }
}
