#ifndef ANALYSIS_WORKER_H
#define ANALYSIS_WORKER_H

#include<QRunnable>
#include<QString>
#include<QDir>
#include<QFileInfoList>
#include "globalvariable.h"
#include <QDebug>

// 子任务类
class AnalysisWorker : public QObject, public QRunnable{
    Q_OBJECT
public:
    AnalysisWorker(QString dirAbslutePath, QString originalDir){
        this->dirAbslutePath = dirAbslutePath;
        this->originalDir = originalDir;
    }

signals:
    void taskFinished(QString rootDirName);

private:
    QString dirAbslutePath;// 文件夹绝对路径
    QString originalDir;// 原始文件夹名称
    long long size = 0;

    void run() override {
        //qDebug() << originalDir << "初始size: " << size;

        addOneTaskToDirSubTaskNumMap(originalDir);

        // 递归遍历
        traversalDir(dirAbslutePath, originalDir);

        // 将结果加进结果map里
        addSizeToMap(originalDir, size);

        minusOneTaskToDirSubTaskNumMap(originalDir);

        //qDebug("%s, 大小:%s, 字节数: %s\n", originalDir.toStdString().data(), MyUtils::formatSize(size).data(), QString::number(size).toStdString().data());

        emit taskFinished(originalDir);
    }

    void traversalDir(QString dirPath, QString originalDir){
        QDir qdir = dirPath;
        // 文件列表
        QFileInfoList fileList = qdir.entryInfoList(QDir::Files | QDir::Hidden);
        if(!fileList.empty()){
            foreach (auto file, fileList) {
                // 跳过快捷方式
                if(!file.exists() || file.isSymLink() || file.isShortcut()){
                    continue;
                }

                // 触发更新label2的信号
                //emit updateLabelSignal2("正在扫描:" + file.fileName());

                if(file.isFile()){
                    size += file.size();
                }else if(file.isDir()){
                    // 递归扫描
                    //traversalDir(file.absoluteFilePath(), originalDir);
                }
            }
        }

        // 文件夹列表
        QFileInfoList dirList = qdir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Hidden);
        if(!dirList.empty()){
            // 超过一个文件夹, 将第二个及之后的文件夹新建多线程任务去处理
            if(dirList.size() > 1){
                for(int i = 1; i < dirList.size(); i++){
                    auto *worker = new AnalysisWorker(dirList[i].absoluteFilePath(), originalDir);
                    worker->setAutoDelete(true);

                    // 添加到线程池
                    getThreadPool()->start(worker);
                }

            }
            // 第一个文件夹继续递归处理
            if(dirList.size() >= 1){
                // 递归扫描
                traversalDir(dirList[0].absoluteFilePath(), originalDir);
            }
        }
    }
};
#endif // ANALYSIS_WORKER_H
