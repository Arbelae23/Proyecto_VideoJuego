#include "Media.h"
#include <QDebug>

Media::Media() {
    cargarMedia();
}

void Media::cargarMedia() {
    // Aquí asignas rutas o inicializas QMediaPlayer si quieres
    cancion_nivel = "";
    // Establecer fondo por defecto para el nivel actual (Nivel 1)
    // Usar recurso embebido para evitar problemas de rutas relativas
    background_nivel = "C:/Users/USER/Documents/GitHub/Proyecto_VideoJuego/CODIGO_VIDEO_GAME/Backgraund/fon1.jpg";

    jugador1_sprite = "C:/Users/USER/Documents/GitHub/Proyecto_VideoJuego/CODIGO_VIDEO_GAME/sprite/Car1.png";
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
