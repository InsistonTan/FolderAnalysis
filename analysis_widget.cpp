#include "analysis_widget.h"
#include "analysis_thread.h"
#include "folder_widget.h"
#include "globalvariable.h"

// 引入全局变量,主窗口
//extern QMainWindow *mainWin;

// 更新进度条的槽函数
void AnalysisWidget::updateProgress(int value){
    progress->setValue(value);
}
// 更新label的槽函数
void AnalysisWidget::updateLabel(QString text){
    label->setText(text);
}
// 更新label2的槽函数
void AnalysisWidget::updateLabel2(QString text){
    label2->setText(text);
}
// 切换主窗口widget到文件夹详情页面的槽函数
void AnalysisWidget::changeToFolderWidget(QString path, QList<FileInfo *> resultList){
    getMainWindow()->setCentralWidget(new FolderWidget(path, resultList));
    currentPageName = PAGE_FOLDER;
}

// 构造函数
AnalysisWidget::AnalysisWidget(QString path){

    progress = new QProgressBar();
    progress->setRange(0, 100);
    progress->setValue(0);
    progress->setStyleSheet("QProgressBar{margin-left:40px;}");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(progress);

    label = new QLabel("正在扫描文件夹");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    label2 = new QLabel("");
    label2->setAlignment(Qt::AlignCenter);
    layout->addWidget(label2);

    layout->setAlignment(Qt::AlignVCenter);
    this->setLayout(layout);

    // 启动一个线程异步处理分析任务
    AnalysisThread *task = new AnalysisThread(path);

    // 绑定信号与槽
    connect(task, &AnalysisThread::updateProgressSignal, this, &AnalysisWidget::updateProgress);
    connect(task, &AnalysisThread::updateLabelSignal, this, &AnalysisWidget::updateLabel);
    connect(task, &AnalysisThread::updateLabelSignal2, this, &AnalysisWidget::updateLabel2);
    connect(task, &AnalysisThread::changeWidgetSignal, this, &AnalysisWidget::changeToFolderWidget);

    // 线程任务开始
    task->start();
}
