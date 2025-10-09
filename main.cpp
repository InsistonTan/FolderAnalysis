#include "mainwindow.h"

#include <QApplication>
#include <QMainWindow>


// 全局变量,缓存文件夹扫描的结果
//QMap<QString, QList<FileInfo *>> RESULT_CACHE;

// 全局变量
//QMainWindow *mainWin;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w;

    w.setFocusPolicy(Qt::ClickFocus);
    w.setWindowTitle("FolderAnalysis v1.0.2");
    w.setWindowIcon(QIcon(":/res/static/icon.png"));
    w.show();

    //mainWin = &w;

    return a.exec();
}
