#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QWidget;
class QPushButton;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onLevel1();
    void onLevel2();
    void onLevel3();
    void onBackToMenu();

private:
    void setupMenu();
    QWidget* createLevelWidget(int level);

    QStackedWidget *stack;
    QWidget *menuWidget;
    QPushButton *backButton;

    // botones del menu (opcional para futuros cambios)
    QPushButton *btnLevel1;
    QPushButton *btnLevel2;
    QPushButton *btnLevel3;
};

#endif // MAINWINDOW_H
