#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    MainWidget(QMainWindow *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;
};
#endif // MAINWINDOW_H
