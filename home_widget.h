#ifndef HOME_WIDGET_H
#define HOME_WIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStorageInfo>
#include <QProgressBar>
#include <QMainWindow>
#include <QEvent>

class HomeWidget : public QWidget{
    Q_OBJECT
public:
    HomeWidget();
    ~HomeWidget();
private:

};

#endif // HOME_WIDGET_H
