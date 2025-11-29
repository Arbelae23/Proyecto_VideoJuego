#include "Media.h"
#include <QDebug>

Media::Media() {
    cargarMedia();
}

void Media::cargarMedia() {
    // Aquí asignas rutas o inicializas QMediaPlayer si quieres
    cancion_nivel = "";
    background_nivel = "";
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
