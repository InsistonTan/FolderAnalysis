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
#include <QTimer>
#include <QFile>
#include "analysis_history_widget.h"

FolderWidget::FolderWidget(QString path, QList<FileInfo *> resultList){
    this->path = path;
    this->resultList = resultList;

    // 当前页面地址, 用于深色模式实时重新加载
    folderUrl = path;

    // 初始化界面
    init();
}

void FolderWidget::init(){
    QVBoxLayout *mainLayout = new QVBoxLayout();

    // 顶部区域的组件
    QWidget *topAreaWidget = new QWidget();
    topAreaWidget->setObjectName("topAreaWidget");
    if(getIsSystemDarkMode()){
        topAreaWidget->setStyleSheet("#topAreaWidget{background-color:rgb(32, 30, 38); border-radius: 8px;} ");
    }else{
        topAreaWidget->setStyleSheet("#topAreaWidget{background-color:rgb(244, 243, 248); border-radius: 8px;} ");
    }

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
            currentPageName = PAGE_HOME;
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
    QPushButton *explorerBtn = new QPushButton("在文件管理器打开");
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

    // 历史分析结果 按钮
    QPushButton *historyBtn = new QPushButton("对比历史分析结果");
    historyBtn->setMaximumWidth(200);
    historyBtn->setStyleSheet("QPushButton{padding: 3 6 3 6; border: 1px solid rgb(200, 200, 200);border-radius: 4px;} "
                              "QPushButton:hover{border: 1px solid rgb(100, 150, 242);}");
    historyBtn->setCursor(Qt::PointingHandCursor);
    // 按钮事件
    QObject::connect(historyBtn, &QPushButton::clicked, [=](){
        AnalysisHistoryWidget *history = new AnalysisHistoryWidget(NULL, path.left(1));

        // 绑定信号
        connect(history, &AnalysisHistoryWidget::selectHistoryFileSignal, this, &FolderWidget::onHistoryFileSelect);

        history->setWindowTitle("已保存的 " + path.left(1) + "盘分析结果");

        history->show();
    });
    topAreaLayout->addWidget(historyBtn);


    // 保存 历史分析结果 按钮
    // 本次打开, 首次进入该磁盘, 弹出询问是否保存扫描结果的弹窗
    if(path.endsWith(":/") && !diskEnterHistory.contains(path) && !resultList.isEmpty()){
        diskEnterHistory.append(path);

        // 保存历史分析结果 按钮
        QPushButton *saveBtn = new QPushButton("保存本次分析结果");
        saveBtn->setMaximumWidth(200);
        saveBtn->setStyleSheet("QPushButton{padding: 3 6 3 6; border: 1px solid rgb(200, 200, 200);border-radius: 4px;background-color:green;color:white;} "
                               "QPushButton:hover{border: 1px solid rgb(100, 150, 242);}");
        saveBtn->setCursor(Qt::PointingHandCursor);
        // 按钮事件
        QObject::connect(saveBtn, &QPushButton::clicked, [=](){
            // 弹窗
            QMessageBox msgBox;
            msgBox.setWindowTitle("保存分析结果");
            msgBox.setText("是否保存本次磁盘分析结果? \n\n如果保存, 当未来磁盘容量发生变化, 可以选择保存的结果进行对比分析");

            // 添加自定义按钮
            QPushButton *confirmBtn = msgBox.addButton(tr("确认"), QMessageBox::YesRole);
            QPushButton *cancelBtn = msgBox.addButton(tr("取消"), QMessageBox::NoRole);

            msgBox.setDefaultButton(confirmBtn); // 设置默认按钮
            msgBox.setIcon(QMessageBox::Question); // 设置图标

            msgBox.exec();

            // 确认
            if (msgBox.clickedButton() == confirmBtn) {
                QDir dir(QDir::homePath() + "/AppData/Local/FolderAnalysis/");
                if (!dir.exists()) {
                    dir.mkpath(".");
                }

                QString saveFileName = QDir::homePath()
                                       + "/AppData/Local/FolderAnalysis/"
                                       + path.left(1) + "盘分析结果_"
                                       + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss")
                                       + ".analysisResult";

                // 保存扫描结果
                QFile file(saveFileName);
                if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QMessageBox::critical(this, "错误", "保存失败!\n\n文件创建失败             ");
                    return;
                }

                auto resultMap = result_cache[path];
                if(resultMap.isEmpty() || resultMap.values().isEmpty()){
                    return;
                }

                QString text;
                for(auto fileInfo : resultMap.values()){
                    text.append(fileInfo->filePath).append(">").append(QString::number(fileInfo->size)).append("\n");
                }


                // 将QString转换为QByteArray写入
                file.write(text.toUtf8());
                file.close();

                QMessageBox::information(this, "提醒", "保存成功!             ");
            }
        });
        topAreaLayout->addWidget(saveBtn);
    }


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

    // 是否需要对比历史结果
    bool isCompareToHistory = false;
    // 选择的历史分析文件不为空, 并且盘符一致
    if(!currentSelectedHistoryFilePath.isEmpty() && currentSelectedHistoryFilePath.contains(path.left(1) + "盘分析结果")){
        isCompareToHistory = true;
    }

    // 创建一个 QStandardItemModel 来存储文件信息
    QStandardItemModel *model = new QStandardItemModel();
    if(isCompareToHistory){
        model->setHorizontalHeaderLabels({"", "文件名", "更新时间", "类型", "大小", "历史大小", "说明"});
        // 对比历史结果, 补全 "历史大小", "说明"
        compareToHistory(resultList);
    }else{
        model->setHorizontalHeaderLabels({"", "文件名", "更新时间", "类型", "大小"});
    }

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

        // 对比历史, 增加列
        if(isCompareToHistory){
            // 创建类型列（文件或文件夹）
            QStandardItem *historySizeItem = new QStandardItem(QString::fromStdString(MyUtils::formatSize(fileInfo->historySize)));
            historySizeItem->setEditable(false);

            // 创建类型列（文件或文件夹）
            QStandardItem *descItem = new QStandardItem(fileInfo->desc);
            descItem->setEditable(false);

            rowItems.append(historySizeItem);
            rowItems.append(descItem);
        }

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
    tableView->setColumnWidth(5, 80);
    tableView->setColumnWidth(6, 100);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);// 点击时选择一整行
    tableView->setFocusPolicy(Qt::NoFocus);// 设置 QTableView 的焦点策略
    tableView->setMouseTracking(true);// 启用鼠标追踪
    tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);

    if(getIsSystemDarkMode()){
        tableView->horizontalHeader()->setStyleSheet("QHeaderView::section {"
                                                     "   padding: 5px 0 -3px 0;"
                                                     "   border: none;"  // 去掉表头的边框
                                                     "}");

        tableView->setStyleSheet("QTableView{"
                                 "}"
                                 "QTableView::item:hover {"
                                 "    background-color: rgb(77, 77, 77);"  // Hover 时的背景色
                                 "    color: white;"             // Hover 时的文本颜色
                                 "}"
                                 "QTableView::item:selected {"
                                 "    background-color: rgb(77, 77, 77);"  // 选中时的背景色
                                 "    color: white;"             // 选中时的文本颜色
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
    }else{
        tableView->horizontalHeader()->setStyleSheet("QHeaderView::section {"
                                                     "   padding: 5px 0 -3px 0;"
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
    }

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

    //qDebug("点击的行号: %d", row);

    if(resultList[row]->isDir){
        // 检查缓存
        MyUtils::checkCache(resultList[row]->filePath);
    }

}

void FolderWidget::onHistoryFileSelect(){
    // 重新初始化
    repaintCentralWidget();
}

void FolderWidget::compareToHistory(QList<FileInfo *> resultList){
    // 当前选择的历史分析文件
    QFile file(currentSelectedHistoryFilePath);
    // 文件不可读
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        currentSelectedHistoryFilePath = "";
        QMessageBox::critical(NULL, "错误", "打开历史分析结果文件失败!");
        return;
    }

    // 创建正则表达式对象, 逻辑:
    // 1.匹配文件夹: 以当前路径开头,且后续除结尾外都没有'/' , 例如当前路径为'E:/abc/' , 将匹配该目录下所有文件夹'E:/abc/xxx/'
    // 2.匹配文件:以当前路径开头, 且后续不存在'/'), 例如当前路径为'E:/abc/' , 将匹配该目录下所有文件'E:/abc/xxx.x'
    QRegularExpression re("(^" + path + "[^/]+/>\\d+$)|(^" + path + "[^/]+$)");
    // 根据正则过滤后的文件信息, key路径, value大小
    QMap<QString, long long> filterMap;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();

        // 进行匹配
        if (re.match(line).hasMatch()) {
            auto splitList = line.split(">");
            filterMap.insert(splitList[0], splitList[1].toLongLong());
        }
    }

    file.close();


    // 补充信息
    for(auto file : resultList){
        if(filterMap.contains(file->filePath)){
            file->historySize = filterMap[file->filePath];
            file->desc = (file->size == file->historySize ? "-"
                                                          : (file->size > file->historySize ? ("↑ " + QString::fromStdString(MyUtils::formatSize(file->size - file->historySize)))
                                                                                            : "↓ " + QString::fromStdString(MyUtils::formatSize(file->historySize - file->size))));
        }else{
            file->desc = "新增";
        }
    }
}




