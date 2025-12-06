#ifndef MEDIA_H
#define MEDIA_H

#include <QString>
#include <QPixmap>
#include <QPainter>

class Media {
public:
    Media();

    QString victoriaImg;
    QString Gameover;
    QString cancion_nivel;
    QString background_nivel;
    QString background_nivel2;
    QString background_nivel3;
    QString jugador1_sprite;

    // Imagen de carretera para scroll en Nivel 1
    QString camino;


    // Sprites del jugador del Nivel 2
    QString jugador2_sprite0;
    QString jugador2_sprite1;
    QString jugador2_sprite2;
    QString jugador2_sprite3;
    QString jugador2_sprite4;
    QString jugador2_sprite5;
    QString jugador2_sprite6;
    QString jugador2_sprite7;


    // Sprites del jugador del Nivel 3 (moto vista trasera)
    QString jugador3_sprite_2; // representa -2
    QString jugador3_sprite_1; // representa -1
    QString jugador3_sprite0;  // representa 0 
    QString jugador3_sprite1;  // representa +1
    QString jugador3_sprite2;  // representa +2
    QString sonido_arresto;
    QString sonido_choque;
    QString sonido_trofeo;
    QString sonido_win;
    QString sonido_loss;

    //Sprites enemigo policia nivel 2
    QString policia_sprite;     // W (arriba)
    QString policia_sprite_S;   // S (abajo)
    QString policia_sprite_A;   // A (izquierda)
    QString policia_sprite_D;   // D (derecha)
    QString policia_sprite_WD;  // diagonal arriba-derecha
    QString policia_sprite_SD;  // diagonal abajo-derecha
    QString policia_sprite_SA;  // diagonal abajo-izquierda
    QString policia_sprite_WA;  // diagonal arriba-izquierda

    //Sprites enemigo bicicleta nivel 2
    QString bicicleta_sprite;
    // Sprite enemigo "rodador" para Nivel 1
    QString rodador_sprite;

    // Sprites de autos enemigos del Nivel 3
    QString lvl3_carro1;
    QString lvl3_carro2;
    QString lvl3_carro3;

    // choque enemigos
    QString Choque;

    //sprite trofeo
    QString trofeo_sprite;

    //Sprite IA
    QString IA;

    void cargarMedia();
    void reproducir_cancionNivel(int nivel);
    void reproducir_background();
    void reproducir_sonidoChoque();
    void reproducir_sonidoTrofeo();
    void reproducir_win();
    void reproducir_loss();

    // Estado y utilidades para dibujar carretera desplazándose
    void setRoadSpeed(int pxPerSec);
    void drawScrollingRoad(QPainter &p, const QRect &area, double dt);

private:
    QPixmap caminoPixmap;
    QPixmap caminoScaled;
    int caminoScaledWidth = 0;
    int caminoScaledHeight = 0;
    double caminoOffsetY = 0.0;
    int caminoSpeedPxPerSec = 120; // velocidad por defecto (px/s)
    int caminoDirection = 1;       // 1 = hacia abajo, -1 = hacia arriba
    
public:
    void setRoadDirectionDown(bool down) { caminoDirection = down ? 1 : -1; }
};

#endif // MEDIA_H

