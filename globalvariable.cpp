#include "globalvariable.h"
#include "home_widget.h"
#include "utils.h"

// 全局变量,缓存文件夹扫描的结果
QMap<QString, QList<FileInfo *>> result_cache;

// 全局变量主窗口
QMainWindow *mainWin;

// 全局变量, 线程池
QThreadPool *threadPool = QThreadPool::globalInstance();

// 存储临时结果map
QMap<QString, FileInfo*> resultMap;
// 互斥锁
QMutex muteX;

// 记录文件夹所拥有的子任务数
QMap<QString, long> dirSubTaskNumMap;
QMutex mutex2;

// 记录有过删除操作的目录和上一级目录(因为文件被删除, 需要对本目录和上一级目录重新扫描)
// QSet<QString> needRefreshDirSet;
// void addToNeedRefreshDirSet(QString dirPath){
//     needRefreshDirSet.insert(dirPath);
// }
// bool isCurrentDirInNeedRefreshDirSet(QString dirPath){
//     return needRefreshDirSet.contains(dirPath);
// }

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
    QMutexLocker locker(&muteX);

    // 汇总结果
    resultMap.value(dir)->size += size;
}

QMap<QString, FileInfo*>* getResultMap(){
    return &resultMap;
}

void insertToTempResultMap(QString filename, FileInfo* fileInfo){
    // QMutexLocker自动管理mutex互斥锁
    QMutexLocker locker(&muteX);
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
void updateResultCache(QString key, QList<FileInfo *> value){
    result_cache[key] = value;
}
QList<FileInfo *> getValueFromCache(QString key){
    return result_cache[key];
}

bool isDarkMode = false;
bool getIsSystemDarkMode(){
    return isDarkMode;
}
void setIsSystemDarkMode(bool val){
    isDarkMode = val;
}

QString currentPageName = "";
QString folderUrl = "";

// 重新绘制中心组件
void repaintCentralWidget(){
    if(currentPageName == PAGE_HOME){
        mainWin->setCentralWidget(new HomeWidget());
    }else if(currentPageName == PAGE_FOLDER){
        MyUtils::checkCache(folderUrl);
    }
}

