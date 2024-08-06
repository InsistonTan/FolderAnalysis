#include "folder_widget.h"
#include "home_widget.h"
#include "utils.h"
#include "analysis_thread.h"
#include "pathinputlinelistener.h"
#include "globalvariable.h"

// 引入全局变量,主窗口
//extern QMainWindow *mainWin;

// 文件夹监听器
class FolderMouseEventListener : public QObject{
private:
    // 监听的目标组件
    QWidget *nameWidget;
    QLabel *updateTime, *type, *size;
public:
    FolderMouseEventListener(QWidget *nameWidget, QLabel *updateTime, QLabel *type, QLabel *size){
        this->nameWidget = nameWidget;
        this->updateTime = updateTime;
        this->type = type;
        this->size = size;
    }

    bool eventFilter(QObject *watched, QEvent *event) override{
        if (event->type() == QEvent::Enter) {
            // 鼠标进入
            const QString styleStr = "QWidget{background-color:rgb(217, 238, 255);}";
            nameWidget->setStyleSheet(styleStr);
            updateTime->setStyleSheet(styleStr);
            type->setStyleSheet(styleStr);
            size->setStyleSheet(styleStr);
            return true;
        }else if(event->type() == QEvent::Leave){
            // 鼠标离开
            const QString styleStr = "QWidget{background-color:rgba(255, 255, 255, 0);}";
            nameWidget->setStyleSheet(styleStr);
            updateTime->setStyleSheet(styleStr);
            type->setStyleSheet(styleStr);
            size->setStyleSheet(styleStr);
            return true;
        }else if(event->type() == QEvent::MouseButtonPress){
            // 鼠标点击事件
            //qDebug() << nameWidget->objectName();

            // 检查缓存
            MyUtils::checkCache(nameWidget->objectName());

            return true;
        }

        return false;
    }
};

FolderWidget::FolderWidget(QString path, QList<FileInfo *> resultList){

    QVBoxLayout *mainLayout = new QVBoxLayout();

    // 顶部区域的组件
    QWidget *topAreaWidget = new QWidget();
    topAreaWidget->setStyleSheet("QWidget{background-color:rgb(244, 243, 248);}");
    QHBoxLayout *topAreaLayout = new QHBoxLayout();
    topAreaWidget->setLayout(topAreaLayout);
    // 返回按钮
    QPushButton *backBtn = new QPushButton("<-");
    topAreaLayout->addWidget(backBtn);
    // 返回按钮事件
    QObject::connect(backBtn, &QPushButton::clicked, [=](){
        // 截取出上一层目录地址
        QString parentPath = path.mid(0, path.lastIndexOf("/"));
        parentPath = parentPath.mid(0, parentPath.lastIndexOf("/") + 1);

        //qDebug() << "path:" << path << "\nparentPath:"  << parentPath;

        // 如果上一次目录的绝对路径已经没有"/"了,代表回到首页
        if(!parentPath.contains("/")){
            getMainWindow()->setCentralWidget(new HomeWidget());
        }else{
            //qDebug() << RESULT_CACHE.keys();

            MyUtils::checkCache(parentPath);
        }
    });
    // 地址栏
    QLineEdit *pathInputLine = new QLineEdit(path);
    pathInputLine->setStyleSheet("QLineEdit{margin:5px 5px 5px 5px;height:20px;}");
    // 地址栏添加回车事件监听
    pathInputLine->installEventFilter(new PathInputLineListener(pathInputLine));
    topAreaLayout->addWidget(pathInputLine);

    // 下方内容区域
    QScrollArea *bottomArea = new QScrollArea();
    // 设置滚动条的样式
    bottomArea->setStyleSheet(R"(
        QScrollBar:vertical {
            border: none;
            background: #f2f2f2;
            width: 5px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: rgb(190, 190, 190);
            border-radius: 8px;
        }
        QScrollBar::sub-line:vertical {
            height: 0px;
        }
    )");
    // 下方widget
    QWidget *bottomAreaWidget = new QWidget();

    // 下方widget的布局
    QGridLayout *bottomLayout = new QGridLayout();
    bottomLayout->setSpacing(0);
    bottomAreaWidget->setLayout(bottomLayout);

    // 固定网格布局的列数
    const int fixedColNum = 4;
    // 标题栏
    QLabel *nameT = new QLabel("名称");
    nameT->setMinimumWidth(200);
    nameT->setContentsMargins(10,0,0,0);
    QLabel *updateDateT = new QLabel("修改日期");
    updateDateT->setMinimumWidth(150);
    QLabel *typeT = new QLabel("类型");
    typeT->setMinimumWidth(80);
    QLabel *sizeT = new QLabel("大小");
    sizeT->setMinimumWidth(80);
    bottomLayout->addWidget(nameT, 0, 0);
    bottomLayout->addWidget(updateDateT, 0, 1);
    bottomLayout->addWidget(typeT, 0, 2);
    bottomLayout->addWidget(sizeT, 0, 3);

    int i = 1;
    for (FileInfo *fileInfo : resultList) {
        QWidget *nameWidget = new QWidget();
        QHBoxLayout *nameLayout = new QHBoxLayout();

        nameLayout->setAlignment(Qt::AlignLeft);
        nameLayout->setContentsMargins(0,2,0,2);
        nameWidget->setLayout(nameLayout);

        QLabel *icon = new QLabel();
        if(fileInfo->isDir){
            icon->setPixmap(QPixmap(":/res/static/folder.png"));
            icon->resize(25, 25);
        }else{
            icon->setPixmap(QPixmap(":/res/static/file.png"));
            icon->resize(26,28);
        }
        QLabel *name = new QLabel(fileInfo->fileName);
        nameLayout->addWidget(icon);
        nameLayout->addWidget(name);

        QLabel *updateDate = new QLabel(fileInfo->updateTime);
        QLabel *type = new QLabel(fileInfo->isDir ? "文件夹" : "文件");
        QLabel *size = new QLabel(QString::fromStdString(MyUtils::formatSize(fileInfo->size)));

        bottomLayout->addWidget(nameWidget, i, 0);
        bottomLayout->addWidget(updateDate, i, 1);
        bottomLayout->addWidget(type, i, 2);
        bottomLayout->addWidget(size, i, 3);

        // 设置监听器
        nameWidget->setObjectName(fileInfo->filePath);
        nameWidget->installEventFilter(new FolderMouseEventListener(nameWidget, updateDate, type, size));
        updateDate->installEventFilter(new FolderMouseEventListener(nameWidget, updateDate, type, size));
        type->installEventFilter(new FolderMouseEventListener(nameWidget, updateDate, type, size));
        size->installEventFilter(new FolderMouseEventListener(nameWidget, updateDate, type, size));

        i++;
    }
    bottomArea->setWidget(bottomAreaWidget);

    mainLayout->addWidget(topAreaWidget);
    mainLayout->addWidget(bottomArea);
    mainLayout->setAlignment(Qt::AlignTop);
    this->setLayout(mainLayout);
}
