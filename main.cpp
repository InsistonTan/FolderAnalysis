#include "mainwindow.h"

#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w;

    w.setFocusPolicy(Qt::ClickFocus);
    w.setWindowTitle("Disk Analysis Tool v1.0.3");
    w.setWindowIcon(QIcon(":/res/static/icon.png"));
    w.show();

    return a.exec();
}
