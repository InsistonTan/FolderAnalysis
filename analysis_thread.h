#ifndef ANALYSIS_THREAD_H
#define ANALYSIS_THREAD_H

#include <QThread>
#include <QObject>
#include <QWidget>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QDir>
#include <QFileInfoList>
#include <QMap>
#include <mutex>
#include "fileinfo.h"
#include <QMainWindow>
#include <QList>
#include "globalvariable.h"

// 引入全局变量,主窗口
//extern QMainWindow *mainWin;

// 引入全局变量,结果缓存
//extern QMap<QString, QList<FileInfo *>> RESULT_CACHE;

class AnalysisThread : public QThread{
    Q_OBJECT

signals:
    // 更新进度条的信号
    void updateProgressSignal(int value);
    // 更新label内容的信号
    void updateLabelSignal(QString text);
    // 切换主窗口widget信号
    void changeWidgetSignal(QString path, QList<FileInfo *> resultList);

private:
    // 目标路径
    QString path;
    // 文件总数
    int totalDirNum = 0;
    // 已分析文件数
    int handledNum = 0;
    // 定义一个互斥锁
    std::mutex lock;
    // 存储结果map
    QMap<QString, FileInfo*> resultMap;
    /**
     * @brief traversalDir 遍历目录下的所有文件
     * @param dirPath 文件夹路径
     */
    void traversalDir(QString dirPath, QString rootDirName){
        QDir qdir = dirPath;
        // NoDotAndDotDot：排除./和../这种上层目录，防止循环不断读取自身
        QFileInfoList fileList = qdir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);
        foreach (auto file, fileList) {
            if(file.isFile()){
                resultMap.value(rootDirName)->size += file.size();
            }else if(file.isDir()){
                // 触发更新label的信号
                //emit updateLabelSignal("正在扫描文件夹:" + rootDirName + ".../" + file.fileName());

                // 递归扫描
                traversalDir(file.absoluteFilePath(), rootDirName);
            }
        }
    }
public:
    AnalysisThread(QString path){
        this->path = path;
    }
    void run() override{
        QDir qdir = path;
        // NoDotAndDotDot：排除./和../这种上层目录，防止循环不断读取自身
        QFileInfoList fileList = qdir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs);
        totalDirNum = qdir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs).size();
        foreach (auto file, fileList) {

            //qDebug() << file.fileName() << ":" << file.absoluteFilePath();

            resultMap.insert(file.fileName(), new FileInfo(
                                                  file.fileName(),
                                                  file.absoluteFilePath() + (file.isDir() ? "/" : ""),
                                                  file.size(),
                                                  file.isDir(),
                                                           file.lastModified().toString("yyyy-MM-dd hh:mm:ss")));
            if(file.isDir()){
                // 触发更新label的信号
                emit updateLabelSignal("正在扫描文件夹:" + file.absoluteFilePath());

                // 扫描文件夹
                traversalDir(file.absoluteFilePath(), file.fileName());

                // 已扫描文件夹数加一
                handledNum++;

                // 触发更新进度条信号
                emit updateProgressSignal((handledNum/(double)totalDirNum)*100);
            }
        }

        // 取出 resultMap 的值,进行排序
        QList<FileInfo *> list = resultMap.values();
        std::sort(list.begin(), list.end(), [](FileInfo *f1, FileInfo *f2){
            return f1->size > f2->size;
        });

        // 将结果加入缓存
        insertToResultMap(path, list);

        // 扫描结束,触发切换主窗口widget信号,进入结果页面
        emit changeWidgetSignal(path, list);

    }
};

#endif // ANALYSIS_THREAD_H
