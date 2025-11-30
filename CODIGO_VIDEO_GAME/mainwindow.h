#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Level1Widget.h"
#include "Level2Widget.h"
#include "Level3Widget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:


private slots:
    void openLevel1();
    void openLevel2();
    void openLevel3();
    void returnToMenu();

private:
    Ui::MainWindow *ui;

    Level1Widget *level1;
    Level2Widget *level2;
    Level3Widget *level3;
};

#endif // MAINWINDOW_H
