#ifndef FOLDER_WIDGET_H
#define FOLDER_WIDGET_H

#include<QWidget>
#include<QVBoxLayout>
#include<QLabel>
#include"fileinfo.h"
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include <QEvent>
#include <QMainWindow>

using namespace std;

class AnalysisWidget;

class FolderWidget : public QWidget{
    Q_OBJECT
private:
    QString path;
    QList<FileInfo *> resultList;

    // 初始化
    void init();

    // 对比历史分析结果
    void compareToHistory(QList<FileInfo *> resultList);
public:
    FolderWidget(QString path, QList<FileInfo *> resultList);
public slots:
    void onRowClicked(const QModelIndex &index);

    void onHistoryFileSelect();
};

#endif // FOLDER_WIDGET_H
