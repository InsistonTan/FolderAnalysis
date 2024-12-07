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
#include "fileinfo.h"
#include <QMainWindow>
#include <QList>
#include "globalvariable.h"
#include<QMutex>
#include<QThreadPool>
#include "analysis_worker.h"
#include <QTimer>


class AnalysisThread : public QThread{
    Q_OBJECT

signals:
    // 更新进度条的信号
    void updateProgressSignal(int value);
    // 更新label内容的信号
    void updateLabelSignal(QString text);
    // 更新label2内容的信号
    void updateLabelSignal2(QString text);
    // 切换主窗口widget信号
    void changeWidgetSignal(QString path, QList<FileInfo *> resultList);

private:
    QMutex handlingDirSetMutex;
    // 正在处理的文件夹
    QSet<QString> handlingDirSet;

    // 目标路径
    QString path;
    // 文件总数
    int totalDirNum = 0;
    // 已分析文件数
    int handledNum = 0;



public slots:
    //收集子任务的结果
    void collectSubTaskResult(QString fileName){
        // 创建定时器对象，定时器触发时检查当前文件夹是否已经扫描完成(子任务数为0)
        QTimer* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [=]() {
            auto subTaskNum = getDirSubTaskNumMap()->value(fileName);
            if(subTaskNum == 0){
                //qDebug("文件夹 %s 扫描结束", fileName.toStdString().data());

                // 已扫描文件夹数加一
                handledNum++;

                // 获得锁
                handlingDirSetMutex.lock();
                // 移除已经扫描完成的文件夹
                handlingDirSet.remove(fileName);
                // 显示新的正在扫描的文件夹
                if(!handlingDirSet.empty()){
                    QSet<QString>::const_iterator it = handlingDirSet.constBegin();

                    if(!it->isNull() && !it->isEmpty()){
                        // 触发更新label的信号
                        emit updateLabelSignal("正在扫描文件夹:" + *it);
                    }
                }
                // 释放锁
                handlingDirSetMutex.unlock();

                // 触发更新进度条信号
                emit updateProgressSignal((handledNum/(double)totalDirNum)*100 - 1);

                // 停止定时器
                timer->stop();
                delete timer;
            } else {
                //qDebug("文件夹 %s 子任务数: %s", fileName.toStdString().data(), std::to_string(subTaskNum).data());
            }
        });

        // 设置定时器的间隔，单位为毫秒
        timer->start(500);  // 每500ms检查一次任务状态
    }

public:
    AnalysisThread(QString path){
        this->path = path;
    }
    void run() override{
        getResultMap()->clear();
        getDirSubTaskNumMap()->clear();

        QDir qdir = path;

        // NoDotAndDotDot：排除./和../这种上层目录，防止循环不断读取自身
        QFileInfoList fileList = qdir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Dirs | QDir::Hidden);
        totalDirNum = fileList.size();

        // 遍历所有文件或文件夹
        foreach (auto file, fileList) {
            // 跳过快捷方式
            if(!file.exists() || file.isSymLink() || file.isShortcut()){
                continue;
            }

            //qDebug() << file.fileName() << ":" << file.absoluteFilePath();

            insertToTempResultMap(file.fileName(), new FileInfo(
                                                  file.fileName(),
                                                  file.absoluteFilePath().replace("\\", "/") + (file.isDir() ? "/" : ""),
                                                  file.size(),
                                                  file.isDir(),
                                                  file.lastModified().toString("yyyy-MM-dd hh:mm"))
                             );

            if(file.isDir()){
                // 获得锁
                handlingDirSetMutex.lock();
                // 添加到正在处理的文件夹set
                handlingDirSet.insert(file.fileName());
                // 释放锁
                handlingDirSetMutex.unlock();

                // 添加当前文件夹到文件夹子任务数量map
                insertToDirSubTaskNumMap(file.fileName(), 0);

                // 新建子任务
                AnalysisWorker *worker = new AnalysisWorker(file.absoluteFilePath(), file.fileName());
                // 任务完成后自动删除
                worker->setAutoDelete(true);

                // 链接信号和槽
                connect(worker, &AnalysisWorker::taskFinished, this, &AnalysisThread::collectSubTaskResult);

                // 将子任务添加到线程池
                getThreadPool()->start(worker);

                // 扫描文件夹
                //traversalDir(file.absoluteFilePath(), file.fileName());
            }else{
                emit updateLabelSignal("正在扫描文件:" + file.fileName());

                // 已扫描文件夹数加一
                handledNum++;

                // 触发更新进度条信号
                emit updateProgressSignal((handledNum/(double)totalDirNum)*100);
            }
        }

        // 等待所有子任务完成
        getThreadPool()->waitForDone();

        // 取出 resultMap 的值,进行排序
        QList<FileInfo *> list = getResultMap()->values();
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
