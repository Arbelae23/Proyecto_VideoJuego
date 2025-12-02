#include "mainwindow.h"

#include <QApplication>

//JUAN TRATA DE QUE TODO LO QUE SE MODOFOQUE EN EL MAIN LO BORRES ANTES DEL COMMIT PARA TRATAR DE NO GENERAR CONFLICTOS

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
