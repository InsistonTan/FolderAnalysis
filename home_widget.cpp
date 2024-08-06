#include "home_widget.h"
#include "utils.h"
#include "pathinputlinelistener.h"
#include "globalvariable.h"

// 引入全局变量,主窗口
//extern QMainWindow *mainWin;

/**
 * @brief The MouseEventListener class
 * 磁盘信息组件的鼠标事件监听类
 */
class HomeMouseEventListener : public QObject{
private:
    // 监听的目标组件
    QWidget *itemWidget;
public:
    HomeMouseEventListener(QWidget *widget){
        this->itemWidget = widget;
    }

    bool eventFilter(QObject *watched, QEvent *event) override{
        if (event->type() == QEvent::Enter) {
            itemWidget->setStyleSheet("QWidget{background-color:rgb(217, 238, 255);}");
            return true;
        }else if(event->type() == QEvent::Leave){
            itemWidget->setStyleSheet("QWidget{background-color:rgba(255, 255, 255, 0);}");
            return true;
        }else if(event->type() == QEvent::MouseButtonPress){
            QString path = itemWidget->objectName();
            // 检查缓存,如果有缓存就直接进入结果页面
            MyUtils::checkCache(path);

            return true;
        }

        return false;
    }
};


HomeWidget::HomeWidget(){
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
    pathInputLine->setStyleSheet("QLineEdit{margin:0px 30px 5px 30px;height:25px;}");
    // 地址栏添加回车事件监听
    pathInputLine->installEventFilter(new PathInputLineListener(pathInputLine));
    contentLayout->addWidget(pathInputLine, 0, 0, 1, 3);

    // 获取电脑磁盘信息
    QList<QStorageInfo> storageInfoList = QStorageInfo::mountedVolumes();

    //QList<QStorageInfo> *storageInfoListP = &storageInfoList;
    //storageInfoListP->append(storageInfoList);

    // 循环生成磁盘信息组件,添加到主布局
    int i = 4;
    for(const QStorageInfo storage : storageInfoList){
        if (storage.isValid()) {

            // 磁盘信息容器
            QWidget *itemWidget = new QWidget();
            // 设置组件名称,名称为磁盘根路径
            itemWidget->setObjectName(storage.rootPath());

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

            // 磁盘信息容器添加鼠标进入监听
            itemWidget->installEventFilter(new HomeMouseEventListener(itemWidget));

            // 添加磁盘信息容器到主布局(计算当前组件在网格中的位置)
            contentLayout->addWidget(itemWidget, i/fixedColNum, i%fixedColNum);

            i++;
        }
    }


    QLabel *about = new QLabel("designed by Insistontan | v0.0.1");
    about->setAlignment(Qt::AlignHCenter);

    mainLayout->addWidget(contentWidget, Qt::AlignTop);
    mainLayout->addStretch();
    mainLayout->addWidget(about);

    // 添加主布局到主界面
    this->setLayout(mainLayout);
}
