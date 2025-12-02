#ifndef MEDIA_H
#define MEDIA_H

#include <QString>

class Media {
public:
    Media();

    QString cancion_nivel;
    QString background_nivel;
    QString jugador1_sprite;
    QString sonido_arresto;
    QString sonido_choque;
    QString sonido_trofeo;
    QString sonido_win;
    QString sonido_loss;

    void cargarMedia();
    void reproducir_cancionNivel(int nivel);
    void reproducir_background();
    void reproducir_sonidoChoque();
    void reproducir_sonidoTrofeo();
    void reproducir_win();
    void reproducir_loss();
};

#endif // MEDIA_H

