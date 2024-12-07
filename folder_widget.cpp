#include "folder_widget.h"
#include "home_widget.h"
#include "utils.h"
#include "pathinputlinelistener.h"
#include "globalvariable.h"
#include "analysis_widget.h"
#include <QProcess>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include "custom_table_view.h"

FolderWidget::FolderWidget(QString path, QList<FileInfo *> resultList){
    this->path = path;
    this->resultList = resultList;

    QVBoxLayout *mainLayout = new QVBoxLayout();

    // 顶部区域的组件
    QWidget *topAreaWidget = new QWidget();
    topAreaWidget->setObjectName("topAreaWidget");
    topAreaWidget->setStyleSheet("#topAreaWidget{background-color:rgb(244, 243, 248); border-radius: 8px;} ");
    QHBoxLayout *topAreaLayout = new QHBoxLayout();
    topAreaWidget->setLayout(topAreaLayout);

    // 返回按钮
    QPushButton *backBtn = new QPushButton("返回");
    backBtn->setMaximumWidth(60);
    backBtn->setStyleSheet("QPushButton{padding: 3 6 3 6; border: 1px solid rgb(200, 200, 200);border-radius: 4px;} "
                           "QPushButton:hover{border: 1px solid rgb(100, 150, 242);}");
    backBtn->setCursor(Qt::PointingHandCursor);
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
    pathInputLine->setStyleSheet("QLineEdit{"
                                 "margin:5px 5px 5px 5px; "
                                 "height:20px; "
                                 "padding: 1 2 1 2; "
                                 "border: 1px solid rgb(200,200,200);"
                                 "border-radius:4px;"
                                 "} "
                                 "QLineEdit:focus { border: 1px solid rgb(100, 150, 242);} "
                                 "QLineEdit:hover { border: 1px solid rgb(100, 150, 242);} "
                                 "");
    // 地址栏添加回车事件监听
    //pathInputLine->installEventFilter(new PathInputLineListener(pathInputLine));
    PathInputLineListener *listener = new PathInputLineListener(pathInputLine);
    connect(pathInputLine, &QLineEdit::returnPressed, listener, &PathInputLineListener::editLineSlot);

    // 顶部区域添加地址栏
    topAreaLayout->addWidget(pathInputLine);

    // 确认按钮
    QPushButton *confirmBtn = new QPushButton("确认");
    confirmBtn->setMaximumWidth(60);
    confirmBtn->setStyleSheet("QPushButton{padding: 3 6 3 6; border: 1px solid rgb(200, 200, 200);border-radius: 4px;} "
                           "QPushButton:hover{border: 1px solid rgb(100, 150, 242);}");
    confirmBtn->setCursor(Qt::PointingHandCursor);
    // 绑定事件
    connect(confirmBtn, &QPushButton::clicked, listener, &PathInputLineListener::editLineSlot);
    topAreaLayout->addWidget(confirmBtn);

    // 刷新按钮
    QPushButton *refreshBtn = new QPushButton("刷新");
    refreshBtn->setMaximumWidth(60);
    refreshBtn->setStyleSheet("QPushButton{padding: 3 6 3 6; border: 1px solid rgb(200, 200, 200);border-radius: 4px;} "
                              "QPushButton:hover{border: 1px solid rgb(100, 150, 242);}");
    refreshBtn->setCursor(Qt::PointingHandCursor);
    // 刷新按钮事件
    QObject::connect(refreshBtn, &QPushButton::clicked, [=](){
        // 主窗口切换进入分析界面
        getMainWindow()->setCentralWidget(new AnalysisWidget(path));
    });
    topAreaLayout->addWidget(refreshBtn);

    // 跳转到资源管理器按钮
    QPushButton *explorerBtn = new QPushButton("在资源管理器打开");
    explorerBtn->setMaximumWidth(200);
    explorerBtn->setStyleSheet("QPushButton{padding: 3 6 3 6; border: 1px solid rgb(200, 200, 200);border-radius: 4px;} "
                              "QPushButton:hover{border: 1px solid rgb(100, 150, 242);}");
    explorerBtn->setCursor(Qt::PointingHandCursor);
    // 跳转到资源管理器按钮事件
    QObject::connect(explorerBtn, &QPushButton::clicked, [=](){
        std::string command = "explorer \"" + path.toStdString() + "\"";  // 使用双引号包裹路径，处理空格
        // 使用 QProcess 执行命令
        QProcess::startDetached(command.data());
    });
    topAreaLayout->addWidget(explorerBtn);



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

    // 创建一个 QTableView
    //QTableView *tableView = new QTableView(this);
    CustomTableView *tableView = new CustomTableView(resultList);

    // 创建一个 QStandardItemModel 来存储文件信息
    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels({"", "文件名", "更新时间", "类型", "大小"});

    // 将文件信息填充到 model 中
    for (FileInfo *fileInfo : resultList) {
        QList<QStandardItem *> rowItems;

        // 创建文件图标列
        QStandardItem *iconItem = new QStandardItem();
        if (fileInfo->isDir) {
            iconItem->setIcon(QIcon(":/res/static/folder.png"));
        } else {
            iconItem->setIcon(QIcon(":/res/static/file.png"));
        }
        iconItem->setData(QVariant::fromValue(fileInfo->fileName));

        // 创建文件名列
        QStandardItem *fileNameItem = new QStandardItem(fileInfo->fileName);
        fileNameItem->setEditable(false);
        fileNameItem->setToolTip(fileInfo->fileName);

        // 创建更新时间列
        QStandardItem *updateDateItem = new QStandardItem(fileInfo->updateTime);
        updateDateItem->setEditable(false);

        // 创建类型列（文件或文件夹）
        QStandardItem *typeItem = new QStandardItem(fileInfo->isDir ? "文件夹" : "文件");
        typeItem->setEditable(false);

        // 创建大小列
        QStandardItem *sizeItem = new QStandardItem(QString::fromStdString(MyUtils::formatSize(fileInfo->size)));
        sizeItem->setEditable(false);

        // 将各列添加到模型中
        rowItems.append(iconItem);
        rowItems.append(fileNameItem);
        rowItems.append(updateDateItem);
        rowItems.append(typeItem);
        rowItems.append(sizeItem);

        // 将当前行添加到 model 中
        model->appendRow(rowItems);
    }

    // 将 model 设置到 listView
    tableView->setModel(model);

    // 设置列表的属性
    tableView->setIconSize(QSize(25, 25));  // 设置图标大小
    //tableView->horizontalHeader()->setStretchLastSection(true); // 自动拉伸最后一列
    tableView->verticalHeader()->hide();  // 隐藏竖向头部
    tableView->setAlternatingRowColors(true);  // 设置交替行颜色
    tableView->resizeColumnsToContents();// 自动调整所有列的宽度
    tableView->setShowGrid(false);  // 显示网格线
    tableView->setColumnWidth(1, 200);
    tableView->setColumnWidth(2, 150);
    tableView->setColumnWidth(3, 80);
    tableView->setColumnWidth(4, 80);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);// 点击时选择一整行
    tableView->setFocusPolicy(Qt::NoFocus);// 设置 QTableView 的焦点策略
    tableView->setMouseTracking(true);// 启用鼠标追踪
    tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    tableView->horizontalHeader()->setStyleSheet("QHeaderView::section {"
                                                 "   border: none;"  // 去掉表头的边框
                                                 "}");

    tableView->setStyleSheet("QTableView{"
                             "}"
                             "QTableView::item:hover {"
                             "    background-color: rgb(229, 243, 255);"  // Hover 时的背景色
                             "    color: #000000;"             // Hover 时的文本颜色
                             "}"
                             "QTableView::item:selected {"
                             "    background-color: rgb(204, 232, 255);"  // 选中时的背景色
                             "    color: #000000;"             // 选中时的文本颜色
                             "}"
                             "QTableView::item:focus {"
                             "    border: none;"  // 取消点击后的边框
                             "    outline: none;"
                             "}"
                             "QTableView QScrollBar:vertical, QTableView QScrollBar:horizontal {"
                             "    background: rgba(0, 0, 0, 0);"
                             "    border-radius: 3px;"   // 圆角滚动条背景
                             "    width: 6px;"          // 垂直滚动条宽度
                             "    height: 6px;"         // 水平滚动条高度
                             "}"
                             "QTableView QScrollBar::handle:vertical, QTableView QScrollBar::handle:horizontal {"
                             "    background: rgba(0, 0, 0, 30%);"   // 半透明滑块
                             "    border-radius: 3px;"                 // 圆角滑块
                             "    min-height: 15px;"                   // 滑块最小高度
                             "    min-width: 15px;"                    // 滑块最小宽度
                             "}"
                             "QTableView QScrollBar::handle:vertical:hover, QTableView QScrollBar::handle:horizontal:hover {"
                             "    background: rgba(0, 0, 0, 50%);"   // 悬停时的滑块颜色
                             "}"
                             "QTableView QScrollBar::add-line:vertical, QTableView QScrollBar::sub-line:vertical, "
                             "QTableView QScrollBar::add-line:horizontal, QTableView QScrollBar::sub-line:horizontal {"
                             "    border: none;"  // 隐藏箭头按钮
                             "    background: none;"  // 不显示箭头按钮背景
                             "}"
                             "QTableView QScrollBar::add-page:vertical, QTableView QScrollBar::sub-page:vertical, "
                             "QTableView QScrollBar::add-page:horizontal, QTableView QScrollBar::sub-page:horizontal {"
                             "    background: none;"  // 隐藏滚动条区域的背景
                             "}");

    // 右键菜单



    connect(tableView, &QTableView::doubleClicked, this, &FolderWidget::onRowClicked);

    mainLayout->addWidget(topAreaWidget);
    //mainLayout->addWidget(bottomArea);
    mainLayout->addWidget(tableView);
    mainLayout->setAlignment(Qt::AlignTop);
    this->setLayout(mainLayout);
}

void FolderWidget::onRowClicked(const QModelIndex &index){
    // 获取点击的行号
    int row = index.row();

    qDebug("点击的行号: %d", row);

    if(resultList[row]->isDir){
        // 检查缓存
        MyUtils::checkCache(resultList[row]->filePath);
    }

}
