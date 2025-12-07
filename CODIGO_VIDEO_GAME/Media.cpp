#include "Media.h"
#include <QDebug>
#include <cmath>

Media::Media() {
    cargarMedia();
}

void Media::cargarMedia() {
    cancion_nivel = "";

    // Fondos
    background_nivel  = ":/Backgraund/Backgraund/fon1.jpg";
    camino            = ":/Backgraund/Backgraund/camino.png";
    background_nivel2 = ":/Backgraund/Backgraund/fon2.png";
    background_nivel3 = ":/Backgraund/Backgraund/fon3.png";
    background_menu   = ":/Backgraund/Backgraund/fon_menu.png";

    // Sprites del jugador
    jugador1_sprite = ":/sprite/sprite/Car1.png";

    jugador2_sprite0 =":/sprite/sprite/C_W.png";
    jugador2_sprite1 =":/sprite/sprite/C_WD.png";
    jugador2_sprite2 =":/sprite/sprite/C_D.png";
    jugador2_sprite3 =":/sprite/sprite/C_SD.png";
    jugador2_sprite4 =":/sprite/sprite/C_S.png";
    jugador2_sprite5 =":/sprite/sprite/C_SA.png";
    jugador2_sprite6 =":/sprite/sprite/C_A.png";
    jugador2_sprite7 =":/sprite/sprite/C_WA.png";


    jugador3_sprite_2 = ":/sprite/sprite/-2m.png";
    jugador3_sprite_1 = ":/sprite/sprite/-1m.png";
    jugador3_sprite0  = ":/sprite/sprite/0m.png";
    jugador3_sprite1  = ":/sprite/sprite/1m.png";
    jugador3_sprite2  = ":/sprite/sprite/2m.png";

    //Sprites enemigos nivel 2

    policia_sprite   = ":/sprite/sprite/Poli_W.png";   // W
    policia_sprite_S = ":/sprite/sprite/Poli_S.png";   // S
    policia_sprite_A = ":/sprite/sprite/Poli_A.png";   // A
    policia_sprite_D = ":/sprite/sprite/Poli_D.png";   // D
    policia_sprite_WD = ":/sprite/sprite/Poli_WD.png"; // WD (arriba-derecha)
    policia_sprite_SD = ":/sprite/sprite/Poli_SD.png"; // SD (abajo-derecha)
    policia_sprite_SA = ":/sprite/sprite/Poli_SA.png"; // SA (abajo-izquierda)
    policia_sprite_WA = ":/sprite/sprite/Poli_WA.png"; // WA (arriba-izquierda)
    bicicleta_sprite = ":/sprite/sprite/Bici.png";
    // Sprite enemigo tipo rodador (Nivel 1)
    rodador_sprite = ":/sprite/sprite/rodador.png";

    // Sprites de autos enemigos del Nivel 3
    lvl3_carro1 = ":/sprite/sprite/lvel3_Carro1.png";
    lvl3_carro2 = ":/sprite/sprite/lvel3_Carro2.png";
    lvl3_carro3 = ":/sprite/sprite/lvel3_Carro3.png";

    //Sprites choque

    Choque = ":/sprite/sprite/Choque.png";
    Gameover = ":/Backgraund/Backgraund/gameover.gif";
    victoriaImg = ":/Backgraund/Backgraund/Win.png";


    //Sprite trofeo
    trofeo_sprite = ":/sprite/sprite/Trofeo.png";


    //Sprite IA
    IA = ":/sprite/sprite/IA.png";


    // Sonidos (cuando los tengas)
    sonido_arresto = "";
    sonido_choque  = "";
    sonido_trofeo  = "";
    sonido_win     = "";
    sonido_loss    = "";
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

// --- Caché mínima con punteros dinámicos ---
Media::~Media() {
    if (l3CarPix) {
        for (int i = 0; i < 3; ++i) {
            delete l3CarPix[i];
        }
        delete[] l3CarPix;
        l3CarPix = nullptr;
    }
    delete trofeoPix;
    trofeoPix = nullptr;
    delete bg3Pix;
    bg3Pix = nullptr;
    delete gameOverPix;
    gameOverPix = nullptr;
    delete victoriaPix;
    victoriaPix = nullptr;
    delete bg1Pix;
    bg1Pix = nullptr;
    delete bg2Pix;
    bg2Pix = nullptr;
    // Nivel 2: enemigos
    delete poliWPix; poliWPix = nullptr;
    delete poliSPix; poliSPix = nullptr;
    delete poliAPix; poliAPix = nullptr;
    delete poliDPix; poliDPix = nullptr;
    delete poliWDPix; poliWDPix = nullptr;
    delete poliSDPix; poliSDPix = nullptr;
    delete poliSAPix; poliSAPix = nullptr;
    delete poliWAPix; poliWAPix = nullptr;
    delete biciPix; biciPix = nullptr;
    delete choquePix; choquePix = nullptr;
}

const QPixmap& Media::getCarro1() {
    if (!l3CarPix) l3CarPix = new QPixmap*[3]{nullptr,nullptr,nullptr};
    if (!l3CarPix[0]) {
        l3CarPix[0] = new QPixmap();
        if (!l3CarPix[0]->load(lvl3_carro1)) {
            *l3CarPix[0] = QPixmap();
            throw MediaLoadError("Failed to load lvl3_carro1");
        }
    }
    return *l3CarPix[0];
}

const QPixmap& Media::getCarro2() {
    if (!l3CarPix) l3CarPix = new QPixmap*[3]{nullptr,nullptr,nullptr};
    if (!l3CarPix[1]) {
        l3CarPix[1] = new QPixmap();
        if (!l3CarPix[1]->load(lvl3_carro2)) {
            *l3CarPix[1] = QPixmap();
            throw MediaLoadError("Failed to load lvl3_carro2");
        }
    }
    return *l3CarPix[1];
}

const QPixmap& Media::getCarro3() {
    if (!l3CarPix) l3CarPix = new QPixmap*[3]{nullptr,nullptr,nullptr};
    if (!l3CarPix[2]) {
        l3CarPix[2] = new QPixmap();
        if (!l3CarPix[2]->load(lvl3_carro3)) {
            *l3CarPix[2] = QPixmap();
            throw MediaLoadError("Failed to load lvl3_carro3");
        }
    }
    return *l3CarPix[2];
}

const QPixmap& Media::getTrofeo() {
    if (!trofeoPix) {
        trofeoPix = new QPixmap();
        if (!trofeoPix->load(trofeo_sprite)) {
            *trofeoPix = QPixmap();
            throw MediaLoadError("Failed to load trofeo_sprite");
        }
    }
    return *trofeoPix;
}

const QPixmap& Media::getBackgroundNivel3() {
    if (!bg3Pix) {
        bg3Pix = new QPixmap();
        if (!bg3Pix->load(background_nivel3)) {
            *bg3Pix = QPixmap();
            throw MediaLoadError("Failed to load background_nivel3");
        }
    }
    return *bg3Pix;
}

const QPixmap& Media::getGameOver() {
    if (!gameOverPix) {
        gameOverPix = new QPixmap();
        if (!gameOverPix->load(Gameover)) {
            *gameOverPix = QPixmap();
            throw MediaLoadError("Failed to load Gameover");
        }
    }
    return *gameOverPix;
}

const QPixmap& Media::getVictoria() {
    if (!victoriaPix) {
        victoriaPix = new QPixmap();
        if (!victoriaPix->load(victoriaImg)) {
            *victoriaPix = QPixmap();
            throw MediaLoadError("Failed to load victoriaImg");
        }
    }
    return *victoriaPix;
}

const QPixmap& Media::getBackgroundNivel1() {
    if (!bg1Pix) {
        bg1Pix = new QPixmap();
        if (!bg1Pix->load(background_nivel)) {
            *bg1Pix = QPixmap();
            throw MediaLoadError("Failed to load background_nivel1");
        }
    }
    return *bg1Pix;
}

const QPixmap& Media::getBackgroundNivel2() {
    if (!bg2Pix) {
        bg2Pix = new QPixmap();
        if (!bg2Pix->load(background_nivel2)) {
            *bg2Pix = QPixmap();
            throw MediaLoadError("Failed to load background_nivel2");
        }
    }
    return *bg2Pix;
}

const QPixmap& Media::getBackgroundMenu() {
    if (!menuBgPix) {
        menuBgPix = new QPixmap();
        if (!menuBgPix->load(background_menu)) {
            *menuBgPix = QPixmap();
            throw MediaLoadError("Failed to load background_menu");
        }
    }
    return *menuBgPix;
}

// --- Nivel 2: enemigos ---
const QPixmap& Media::getPoliciaW() {
    if (!poliWPix) {
        poliWPix = new QPixmap();
        if (!poliWPix->load(policia_sprite)) { *poliWPix = QPixmap(); throw MediaLoadError("Failed to load policia_sprite W"); }
    }
    return *poliWPix;
}

const QPixmap& Media::getPoliciaS() {
    if (!poliSPix) {
        poliSPix = new QPixmap();
        if (!poliSPix->load(policia_sprite_S)) { *poliSPix = QPixmap(); throw MediaLoadError("Failed to load policia_sprite S"); }
    }
    return *poliSPix;
}

const QPixmap& Media::getPoliciaA() {
    if (!poliAPix) {
        poliAPix = new QPixmap();
        if (!poliAPix->load(policia_sprite_A)) { *poliAPix = QPixmap(); throw MediaLoadError("Failed to load policia_sprite A"); }
    }
    return *poliAPix;
}

const QPixmap& Media::getPoliciaD() {
    if (!poliDPix) {
        poliDPix = new QPixmap();
        if (!poliDPix->load(policia_sprite_D)) { *poliDPix = QPixmap(); throw MediaLoadError("Failed to load policia_sprite D"); }
    }
    return *poliDPix;
}

const QPixmap& Media::getPoliciaWD() {
    if (!poliWDPix) {
        poliWDPix = new QPixmap();
        if (!poliWDPix->load(policia_sprite_WD)) { *poliWDPix = QPixmap(); throw MediaLoadError("Failed to load policia_sprite WD"); }
    }
    return *poliWDPix;
}

const QPixmap& Media::getPoliciaSD() {
    if (!poliSDPix) {
        poliSDPix = new QPixmap();
        if (!poliSDPix->load(policia_sprite_SD)) { *poliSDPix = QPixmap(); throw MediaLoadError("Failed to load policia_sprite SD"); }
    }
    return *poliSDPix;
}

const QPixmap& Media::getPoliciaSA() {
    if (!poliSAPix) {
        poliSAPix = new QPixmap();
        if (!poliSAPix->load(policia_sprite_SA)) { *poliSAPix = QPixmap(); throw MediaLoadError("Failed to load policia_sprite SA"); }
    }
    return *poliSAPix;
}

const QPixmap& Media::getPoliciaWA() {
    if (!poliWAPix) {
        poliWAPix = new QPixmap();
        if (!poliWAPix->load(policia_sprite_WA)) { *poliWAPix = QPixmap(); throw MediaLoadError("Failed to load policia_sprite WA"); }
    }
    return *poliWAPix;
}

const QPixmap& Media::getBicicleta() {
    if (!biciPix) {
        biciPix = new QPixmap();
        if (!biciPix->load(bicicleta_sprite)) { *biciPix = QPixmap(); throw MediaLoadError("Failed to load bicicleta_sprite"); }
    }
    return *biciPix;
}

const QPixmap& Media::getChoque() {
    if (!choquePix) {
        choquePix = new QPixmap();
        if (!choquePix->load(Choque)) { *choquePix = QPixmap(); throw MediaLoadError("Failed to load Choque sprite"); }
    }
    return *choquePix;
}
