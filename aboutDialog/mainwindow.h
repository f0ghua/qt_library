#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "AboutDialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    AboutDialog *m_aboutDialog;
};

#endif // MAINWINDOW_H
