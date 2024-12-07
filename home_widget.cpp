#include "home_widget.h"
#include "utils.h"
#include "pathinputlinelistener.h"
#include<QPushButton>

QString currentClickStorage;

/**
 * @brief The MouseEventListener class
 * 磁盘信息组件的鼠标事件监听类
 */
class HomeMouseEventListener : public QObject{
private:
    // 监听的目标组件
    QWidget *itemWidget;
    // 所有磁盘的QWidget组件列表
    QList<QWidget *> storageWidgetList;
public:
    HomeMouseEventListener(QWidget *widget, QList<QWidget *> storageWidgetList){
        this->itemWidget = widget;
        this->storageWidgetList = storageWidgetList;
    }

    bool eventFilter(QObject *watched, QEvent *event) override{
        if(event->type() == QEvent::MouseButtonPress){
            currentClickStorage = itemWidget->objectName();
            itemWidget->setStyleSheet("QWidget{background-color:rgb(204, 232, 255);}");

            for (int i = 0; i < storageWidgetList.size(); ++i) {
                QWidget *widget = storageWidgetList.at(i);
                if(widget->objectName() != itemWidget->objectName()){
                    widget->setStyleSheet("QWidget{background-color:rgba(255, 255, 255, 0);}");
                }
            }
        }
        else if (event->type() == QEvent::Enter && currentClickStorage != itemWidget->objectName()) {
            itemWidget->setStyleSheet("QWidget{background-color:rgb(217, 238, 255);}");
            //itemWidget->setCursor(Qt::PointingHandCursor);
            return true;
        }else if(event->type() == QEvent::Leave && currentClickStorage != itemWidget->objectName()){
            itemWidget->setStyleSheet("QWidget{background-color:rgba(255, 255, 255, 0);}");
            return true;
        }else if(event->type() == QEvent::MouseButtonDblClick){
            QString path = itemWidget->objectName();
            // 检查缓存,如果有缓存就直接进入结果页面
            MyUtils::checkCache(path);

            return true;
        }

        return false;
    }
};

HomeWidget::~HomeWidget(){}

HomeWidget::HomeWidget(){
    currentClickStorage = "";

    // 主界面布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *contentWidget = new QWidget();
    QGridLayout *contentLayout = new QGridLayout(this);
    contentWidget->setLayout(contentLayout);

    // 网格布局固定列数
    const int fixedColNum = 4;
    contentLayout->setAlignment(Qt::AlignTop);

    // 地址栏
    QLineEdit *pathInputLine = new QLineEdit("");
    pathInputLine->setPlaceholderText("输入目录路径, 回车确认");
    pathInputLine->setStyleSheet("QLineEdit{"
                                 "margin:0px 30px 5px 30px; "
                                 "height:25px; "
                                 "padding-left:5px; "
                                 "border-radius: 4px;"
                                 "border: 1px solid rgb(200, 200, 200);"
                                 "} "
                                 "QLineEdit:focus {border: 1px solid rgb(100, 150, 242);} "
                                 "QLineEdit:hover{border: 1px solid rgb(100, 150, 242);} ");

    // 地址栏添加回车事件监听
    PathInputLineListener *listener = new PathInputLineListener(pathInputLine);
    connect(pathInputLine, &QLineEdit::returnPressed, listener, &PathInputLineListener::editLineSlot);

    contentLayout->addWidget(pathInputLine, 0, 0, 1, 3);

    // 获取电脑磁盘信息
    QList<QStorageInfo> storageInfoList = QStorageInfo::mountedVolumes();

    //QList<QStorageInfo> *storageInfoListP = &storageInfoList;
    //storageInfoListP->append(storageInfoList);

    QList<QWidget *> storageWidgetList;

    // 循环生成磁盘信息组件,添加到主布局
    int i = 4;
    for(const QStorageInfo storage : storageInfoList){
        if (storage.isValid()) {

            // 磁盘信息容器
            QWidget *itemWidget = new QWidget();

            // 设置组件名称,名称为磁盘根路径
            itemWidget->setObjectName(storage.rootPath().replace("\\", "/").toUpper());

            // 给磁盘信息容器使用的水平布局
            QHBoxLayout *innerLayout = new QHBoxLayout();
            innerLayout->setAlignment(Qt::AlignLeft);

            // 磁盘图标
            QLabel *picLabel = new QLabel();
            QPixmap pixmap(":/res/static/disk-icon.png");
            picLabel->setPixmap(pixmap);
            picLabel->resize(65, 68);

            // 容器内部右侧子容器
            QWidget *innerRightWidget = new QWidget();
            // 给子容器使用的垂直布局
            QVBoxLayout *innerRightLayout = new QVBoxLayout();
            // 信息组件(磁盘名称和盘符)
            QLabel *label1 = new QLabel(storage.name() + "(" + storage.rootPath().removeLast() + ")");
            label1->setMaximumWidth(200);
            // 进度条组件
            QProgressBar *progressBar = new QProgressBar();
            progressBar->setMaximumWidth(210);
            progressBar->setRange(0, 100);
            progressBar->setValue((storage.bytesTotal() - storage.bytesAvailable())/(double)storage.bytesTotal() * 100);
            // 信息组件(磁盘空间占用信息)
            QLabel *label2 = new QLabel((MyUtils::formatSize(storage.bytesAvailable()) + " 可用, 共 "
                                         + MyUtils::formatSize(storage.bytesTotal())).data());
            label2->setStyleSheet("QLabel{color:rgb(109, 109, 109);}");

            // 子容器设置垂直布局
            innerRightWidget->setLayout(innerRightLayout);
            // 垂直布局添加组件
            innerRightLayout->addWidget(label1);
            innerRightLayout->addWidget(progressBar);
            innerRightLayout->addWidget(label2);

            // 磁盘信息容器内部使用水平布局
            itemWidget->setLayout(innerLayout);
            // 水平布局添加磁盘图标组件
            innerLayout->addWidget(picLabel);
            // 水平布局添加右侧子容器
            innerLayout->addWidget(innerRightWidget);

            // 组件添加到列表
            storageWidgetList.append(itemWidget);
        }
    }

    for(auto item : storageWidgetList){
        // 磁盘信息容器添加鼠标进入监听
        item->installEventFilter(new HomeMouseEventListener(item, storageWidgetList));

        // 添加磁盘信息容器到主布局(计算当前组件在网格中的位置)
        contentLayout->addWidget(item, i/fixedColNum, i%fixedColNum);

        i++;
    }


    QLabel *about = new QLabel("<a href=\"https://github.com/InsistonTan/FolderAnalysis\">项目地址: InsistonTan/FolderAnalysis</a>");
    about->setOpenExternalLinks(true);
    about->setAlignment(Qt::AlignHCenter);

    QPushButton *donate = new QPushButton("支持作者");
    donate->setMaximumWidth(100);

    connect(donate, &QPushButton::clicked, [=]{
        QMainWindow *window = new QMainWindow();
        window->setWindowTitle("请作者喝杯奶茶, 感谢您的支持和鼓励");
        window->setFixedSize(400, 300);

        // 创建一个 QLabel 来显示图片
        QLabel* label = new QLabel();
        QPixmap pixmap(":/res/static/shoukuanma.jpg");  // 替换为你的图片路径
        pixmap = pixmap.scaled(400, 300, Qt::KeepAspectRatio);
        label->setPixmap(pixmap);
        label->setAlignment(Qt::AlignCenter);  // 设置图片居中显示

        // 创建一个 QWidget 作为主窗口的中央部件
        QWidget* centralWidget = new QWidget();

        // 创建一个 QVBoxLayout 布局管理器
        QVBoxLayout* layout = new QVBoxLayout(centralWidget);

        // 将 QLabel 添加到布局中
        layout->addWidget(label);

        // 设置布局为中央部件的布局
        centralWidget->setLayout(layout);

        // 设置中央部件
        window->setCentralWidget(centralWidget);

        window->show();
    });

    mainLayout->addWidget(contentWidget, Qt::AlignTop);
    mainLayout->addStretch();
    mainLayout->addWidget(about);
    mainLayout->addWidget(donate, 0, Qt::AlignCenter);


    // 添加主布局到主界面
    this->setLayout(mainLayout);
}
