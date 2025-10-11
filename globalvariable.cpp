#include "globalvariable.h"
#include "home_widget.h"
#include "utils.h"
#include <QFileInfo>

// 全局变量,缓存文件夹扫描的结果
QMap<QString, QMap<QString, FileInfo*>> result_cache;

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

void addSizeToMapAllDir(QString dir, long long size){
    // QMutexLocker自动管理mutex互斥锁
    QMutexLocker locker(&muteX);

    // 从dir中分离出每层的目录
    auto dirSplitList = dir.split("/");

    // dir的父级目录全都加上 size
    QString tempDir = "";
    for(auto subDir : dirSplitList){
        if(!subDir.isEmpty()){
            tempDir += subDir + "/";

            if(!resultMap.contains(tempDir)){
                auto dirInfo = QFileInfo(tempDir);
                if(dirInfo.exists()){
                    resultMap.insert(tempDir, new FileInfo(dirInfo.dir().dirName(),
                                                           dirInfo.absoluteFilePath().replace("\\", "/"),
                                                           dirInfo.size(),
                                                           true,
                                                           dirInfo.lastModified().toString("yyyy-MM-dd hh:mm")));
                }

            }

            if(resultMap.contains(tempDir)){
                auto a = resultMap.value(tempDir);
                // 汇总结果
                resultMap.value(tempDir)->size += size;
            }
        }
    }
}

// 从结果map中获取需要的文件信息
QList<FileInfo *> getNeedFileListFromResult(QString path, QMap<QString, FileInfo*> result){
    QDir qdir = path;

    // NoDotAndDotDot：排除./和../这种上层目录，防止循环不断读取自身
    QFileInfoList fileList = qdir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs | QDir::Hidden);

    // 展示的文件夹列表, 即path的子级
    QList<QString> needToShowFileList;

    // 遍历所有文件或文件夹
    foreach (auto file, fileList) {
        // 跳过快捷方式
        if(!file.exists() || file.isSymLink()){
            continue;
        }

        // 保存需要展示的文件列表
        needToShowFileList.append(file.absoluteFilePath().replace("\\", "/") + (file.isDir() ? "/" : ""));
    }

    // 最终返回的排序之后结果
    QList<FileInfo *> res;
    for(auto filePath : needToShowFileList){
        if (result.contains(filePath)) {
            res.append(result[filePath]);
        }
    }

    if(res.size() > 1){
        // 根据大小倒序排序
        std::sort(res.begin(), res.end(), [](FileInfo *f1, FileInfo *f2){
            return f1->size > f2->size;
        });
    }

    return res;
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


void insertToResultCache(QString key, QMap<QString, FileInfo*> value){
    result_cache.insert(key, value);
}
void updateResultCache(QString key, QMap<QString, FileInfo*> value){
    // 清空QMap，移除所有键值对
    result_cache[key].clear();
    // 替换新value
    result_cache[key] = value;
}
QMap<QString, FileInfo*> getValueFromCache(QString key){
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

QList<QString> diskEnterHistory;

QString currentSelectedHistoryFilePath = "";



