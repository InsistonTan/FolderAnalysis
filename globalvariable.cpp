#include "globalvariable.h"

// 全局变量,缓存文件夹扫描的结果
QMap<QString, QList<FileInfo *>> result_cache;

// 全局变量主窗口
QMainWindow *mainWin;

// 全局变量, 线程池
QThreadPool *threadPool = QThreadPool::globalInstance();

// 存储临时结果map
QMap<QString, FileInfo*> resultMap;
// 互斥锁
QMutex mutex;

// 记录文件夹所拥有的子任务数
QMap<QString, long> dirSubTaskNumMap;
QMutex mutex2;

QMap<QString, long>* getDirSubTaskNumMap(){
    return &dirSubTaskNumMap;
}
void insertToDirSubTaskNumMap(QString dir, long num){
    // QMutexLocker自动管理mutex互斥锁
    QMutexLocker locker(&mutex2);

    dirSubTaskNumMap.insert(dir, num);
}
void addOneTaskToDirSubTaskNumMap(QString dir){
    // QMutexLocker自动管理mutex互斥锁
    QMutexLocker locker(&mutex2);

    //auto item = dirSubTaskNumMap.find(dir);
    //if(item != dirSubTaskNumMap.end()){
    dirSubTaskNumMap[dir] += 1;
    //}
}
void minusOneTaskToDirSubTaskNumMap(QString dir){
    // QMutexLocker自动管理mutex互斥锁
    QMutexLocker locker(&mutex2);

    //auto item = dirSubTaskNumMap.find(dir);
    //if(item != dirSubTaskNumMap.end()){
    dirSubTaskNumMap[dir] -= 1;
    //}
}

void addSizeToMap(QString dir, long long size){
    // QMutexLocker自动管理mutex互斥锁
    QMutexLocker locker(&mutex);

    // 汇总结果
    resultMap.value(dir)->size += size;
}

QMap<QString, FileInfo*>* getResultMap(){
    return &resultMap;
}

void insertToTempResultMap(QString filename, FileInfo* fileInfo){
    // QMutexLocker自动管理mutex互斥锁
    QMutexLocker locker(&mutex);
    resultMap.insert(filename, fileInfo);
}


QThreadPool* getThreadPool(){
    return threadPool;
}

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
