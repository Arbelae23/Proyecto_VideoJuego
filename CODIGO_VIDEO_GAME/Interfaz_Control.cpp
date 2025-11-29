#include "Interfaz_Control.h"

Interfaz_Control::Interfaz_Control(QObject *parent)
    : QObject(parent), Current_Level(0)
{}

void Interfaz_Control::Mostrar_menu() {
    Current_Level = 0;
    emit levelChanged(0);
}

void Interfaz_Control::play_level1() {
    Current_Level = 1;
    emit levelChanged(1);
}

void Interfaz_Control::play_level2() {
    Current_Level = 2;
    emit levelChanged(2);
}

void Interfaz_Control::play_level3() {
    Current_Level = 3;
    emit levelChanged(3);
}
