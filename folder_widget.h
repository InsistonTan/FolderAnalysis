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

public:
    FolderWidget(QString path, QList<FileInfo *> resultList);
};

#endif // FOLDER_WIDGET_H
