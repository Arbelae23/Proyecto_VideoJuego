#ifndef OBJETO_INTERACTIVO_H
#define OBJETO_INTERACTIVO_H

#include <QRect>
#include <QPainter>
#include <QString>
#include <QPixmap>
#include <QPoint>
#include <QSize>

enum TipoMovimiento {
    TM_Stationary,
    TM_Seno,
    TM_Orbital,
    TM_Pendular,
    TM_Linear
};

class Objeto_Interactivo {
public:
    Objeto_Interactivo();

    // atributos (según diagrama)
    QPoint spawn_xy;
    QString skin;
    QString tipo_objeto;
    TipoMovimiento tipo_movimiento;
    double tiempo_pantalla; // vida en pantalla (segundos)
    QSize tamaño;

    // estado
    bool activo;
    QRect bounds;  // area actual (pixeles)

    QPixmap sprite;

    // metodos
    virtual void update(double dt);
    virtual void draw(QPainter &p);
    virtual void aparecer(const QPoint &pos);
    virtual void desaparecer();
    virtual void kill();
    virtual void sumarTrofeo(int cantidad = 1);

    // helpers
    QRect getBounds() const { return bounds; }
};

#endif // OBJETO_INTERACTIVO_H
