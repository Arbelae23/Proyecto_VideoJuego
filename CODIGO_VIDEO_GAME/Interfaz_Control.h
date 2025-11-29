#ifndef INTERFAZ_CONTROL_H
#define INTERFAZ_CONTROL_H

#include <QObject>

class Interfaz_Control : public QObject {
    Q_OBJECT
public:
    explicit Interfaz_Control(QObject *parent = nullptr);

    // Estado principal
    int Current_Level;

    // Control de menú
    void Mostrar_menu();
    void play_level1();
    void play_level2();
    void play_level3();

signals:
    // señales útiles para la UI
    void levelChanged(int newLevel);
};

#endif // INTERFAZ_CONTROL_H
