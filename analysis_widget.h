#ifndef ANALYSIS_WIDGET_H
#define ANALYSIS_WIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QDir>
#include <QFileInfoList>
#include <QMap>
#include <QMainWindow>
#include "fileinfo.h"

using namespace std;


class AnalysisWidget : public QWidget{
    Q_OBJECT

private:
    QProgressBar *progress;
    QLabel *label;
    QLabel *label2;

public slots:
    // 更新进度条的槽函数
    void updateProgress(int value);
    // 更新label的槽函数
    void updateLabel(QString text);
    // 更新label2的槽函数
    void updateLabel2(QString text);
    // 切换主窗口widget到文件夹详情页面的槽函数
    void changeToFolderWidget(QString path, QList<FileInfo *> resultList);

public:
    AnalysisWidget(QString path);

};



#endif // ANALYSIS_WIDGET_H
