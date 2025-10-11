#include "analysis_history_widget.h"
#include "ui_analysis_history_widget.h"
#include <QDir>
#include <QProcess>
#include <QLabel>
#include "utils.h"
#include "globalvariable.h"

AnalysisHistoryWidget::AnalysisHistoryWidget(QWidget *parent, QString diskSymbol)
    : QWidget(parent)
    , ui(new Ui::AnalysisHistoryWidget)
{
    ui->setupUi(this);

    this->diskSymbol = diskSymbol;

    this->setWindowTitle("已保存的历史分析结果");

    // 主布局, 垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout();
    // 设置顶部对齐
    mainLayout->setAlignment(Qt::AlignTop);
    ui->scrollAreaWidgetContents->setLayout(mainLayout);

    // 获取已保存的历史分析结果文件
    auto fileList = getHistoryResultFiles();

    int i = 0;
    // 遍历fileList, 生成组件添加到主布局
    for(auto file : fileList){
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout();
        itemWidget->setLayout(layout);

        // 文件信息
        QLabel *nameLabel = new QLabel();
        nameLabel->setToolTip(file.fileName());
        nameLabel->setText(QString::number(++i) + "." + file.completeBaseName() + "    日期: " + file.birthTime().toString("yyyy-MM-dd HH:mm:ss") + "    大小: " + MyUtils::formatSize(file.size()).data());
        layout->addWidget(nameLabel);

        // 历史分析结果 按钮
        QPushButton *selectBtn = new QPushButton("选择该结果进行对比");
        selectBtn->setMaximumWidth(130);
        selectBtn->setStyleSheet("QPushButton{padding: 3 6 3 6; border: 1px solid rgb(200, 200, 200);border-radius: 4px;} "
                                  "QPushButton:hover{border: 1px solid rgb(100, 150, 242);}");
        selectBtn->setCursor(Qt::PointingHandCursor);
        // 按钮事件
        QObject::connect(selectBtn, &QPushButton::clicked, [=](){
            currentSelectedHistoryFilePath = file.absoluteFilePath();
            emit selectHistoryFileSignal();
            this->hide();
        });
        layout->addWidget(selectBtn);


        // 添加组件到主布局
        mainLayout->addWidget(itemWidget);
    }
}

QList<QFileInfo> AnalysisHistoryWidget::getHistoryResultFiles(){
    QList<QFileInfo> fileList;

    // 获取软件数据保存目录下所有文件
    QDir dir(QDir::homePath() + "/AppData/Local/FolderAnalysis/");
    if(dir.exists()){
        // 获取目录下带有当前盘符的.analysisResult文件
        QFileInfoList fileInfoList = dir.entryInfoList(
            QStringList() << diskSymbol + "盘分析结果*.analysisResult",
            QDir::Files,
            QDir::Name
            );

        if(!fileInfoList.isEmpty()){
            fileList.append(fileInfoList);
        }
    }

    return fileList;
}

AnalysisHistoryWidget::~AnalysisHistoryWidget()
{
    delete ui;
}

void AnalysisHistoryWidget::on_pushButton_2_clicked()
{
    std::string command = "explorer \"" + (QDir::homePath() + "/AppData/Local/FolderAnalysis/").toStdString() + "\"";  // 使用双引号包裹路径，处理空格
    // 使用 QProcess 执行命令
    QProcess::startDetached(command.data());
}

