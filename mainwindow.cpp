#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStorageInfo>
#include <QProgressBar>
#include "home_widget.h"
#include "globalvariable.h"
#include <QMenuBar>

using namespace std;

// 主窗口
//static QMainWindow *mainWindow;
// 程序首页
//static QWidget *homeWidget;

MainWidget::MainWidget(QMainWindow *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    this->setStyleSheet("QWidget { background-color:white; }");

    // 创建首页界面
    QWidget *centralWidget = new HomeWidget();

    // 设置中心组件
    this->setCentralWidget(centralWidget);

    // this为主窗口
    //mainWindow = this;
    setMainWindow(this);

    // QMap<QString, QList<FileInfo *>> tempMap = {{"tempKey",{}}};
    // GlobalVariable::setResultCache(&tempMap);
    // GlobalVariable::getResultCache()->remove("tempKey");

    // 设置当前界面为首页
    //homeWidget = centralWidget;
}

MainWidget::~MainWidget()
{
    delete ui;

}




