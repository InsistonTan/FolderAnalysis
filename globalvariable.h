#ifndef GLOBALVARIABLE_H
#define GLOBALVARIABLE_H

#include<QString>
#include<QMap>
#include<QMainWindow>
#include"fileinfo.h"

// 全局变量,缓存文件夹扫描的结果
extern QMap<QString, QList<FileInfo *>> result_cache;
QMap<QString, QList<FileInfo *>> getResultCache();
void insertToResultMap(QString key, QList<FileInfo *> value);

// 全局变量
extern QMainWindow *mainWin;
QMainWindow* getMainWindow();
void setMainWindow(QMainWindow *win);

#endif // GLOBALVARIABLE_H
