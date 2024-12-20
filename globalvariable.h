#ifndef GLOBALVARIABLE_H
#define GLOBALVARIABLE_H

#include<QString>
#include<QMap>
#include<QMainWindow>
#include"fileinfo.h"
#include<QThreadPool>
#include<QMutex>

// 全局变量,缓存文件夹扫描的结果
extern QMap<QString, QList<FileInfo *>> result_cache;
QMap<QString, QList<FileInfo *>> getResultCache();
void insertToResultMap(QString key, QList<FileInfo *> value);
void updateResultCache(QString key, QList<FileInfo *> value);
QList<FileInfo *> getValueFromCache(QString key);

// 全局变量, 主窗口
extern QMainWindow *mainWin;
QMainWindow* getMainWindow();
void setMainWindow(QMainWindow *win);

// 全局变量, 线程池
extern QThreadPool *threadPool;
QThreadPool* getThreadPool();

// 存储临时结果map
extern QMap<QString, FileInfo*> resultMap;
// 互斥锁
extern QMutex mutex;
void addSizeToMap(QString dir, long long size);
QMap<QString, FileInfo*>* getResultMap();
void insertToTempResultMap(QString filename, FileInfo* fileInfo);

// 记录文件夹所拥有的子任务数
extern QMap<QString, long> dirSubTaskNumMap;
QMap<QString, long>* getDirSubTaskNumMap();
void insertToDirSubTaskNumMap(QString dir, long num);
void addOneTaskToDirSubTaskNumMap(QString dir);
void minusOneTaskToDirSubTaskNumMap(QString dir);

// 记录有过删除操作的目录和上一级目录(因为文件被删除, 需要对本目录和上一级目录重新扫描)
// extern QSet<QString> needRefreshDirSet;
// void addToNeedRefreshDirSet(QString dirPath);
// bool isCurrentDirInNeedRefreshDirSet(QString dirPath);

#endif // GLOBALVARIABLE_H
