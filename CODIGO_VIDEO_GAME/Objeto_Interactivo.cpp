#include "Objeto_Interactivo.h"

Objeto_Interactivo::Objeto_Interactivo()
    : spawn_xy(0,0), skin(""), tipo_objeto("generic"), tipo_movimiento(TM_Stationary),
    tiempo_pantalla(0), tamaño(40,40), activo(false)
{
    bounds = QRect(spawn_xy, tamaño);
}

void Objeto_Interactivo::update(double dt) {
    if (!activo) return;
    // Por defecto no se mueve
    if (tiempo_pantalla > 0) {
        tiempo_pantalla -= dt;
        if (tiempo_pantalla <= 0) desaparecer();
    }
}

void Objeto_Interactivo::draw(QPainter &p) {
    if (!activo) return;
    p.setBrush(Qt::yellow);
    p.drawRect(bounds);
}

void Objeto_Interactivo::aparecer(const QPoint &pos) {
    activo = true;
    spawn_xy = pos;
    bounds = QRect(pos, tamaño);
}

void Objeto_Interactivo::desaparecer() {
    activo = false;
}

void Objeto_Interactivo::kill() {
    activo = false;
}

void Objeto_Interactivo::sumarTrofeo(int) {
    // implementar según Interacciones
}
