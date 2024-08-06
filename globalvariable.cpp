#include "globalvariable.h"

// 全局变量,缓存文件夹扫描的结果
QMap<QString, QList<FileInfo *>> result_cache;

// 全局变量
QMainWindow *mainWin;

QMainWindow* getMainWindow(){
    return mainWin;
}
void setMainWindow(QMainWindow *win){
    mainWin = win;
}

QMap<QString, QList<FileInfo *>> getResultCache(){
    return result_cache;
}
void insertToResultMap(QString key, QList<FileInfo *> value){
    result_cache.insert(key, value);
}
