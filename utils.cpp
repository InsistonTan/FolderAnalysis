#include "utils.h"
#include <fileinfo.h>
#include "folder_widget.h"
#include "analysis_widget.h"
#include <QMainWindow>
#include <iomanip>
#include <sstream>
#include "globalvariable.h"
#include <QApplication>

// 引入全局变量
// 结果缓存
//extern QMap<QString, QList<FileInfo *>> RESULT_CACHE;
// 主窗口
//extern QMainWindow *mainWin;

void MyUtils::checkCache(QString path){
    // 检查缓存,如果有缓存就直接进入结果页面
    // path.left(3)截取盘符
    if(result_cache.contains(path.left(3))){
        // 设置鼠标指针为忙碌
        QApplication::setOverrideCursor(Qt::BusyCursor);

        // getValueFromCache(path.left(3)) 根据盘符获取该盘缓存
        QList<FileInfo *> cacheList = getNeedFileListFromResult(path, getValueFromCache(path.left(3)));
        if(!cacheList.empty()){
            // 初始化文件夹列表页面
            auto *folder = new FolderWidget(path, cacheList);

            // 恢复鼠标指针
            QApplication::restoreOverrideCursor();

            // 直接进入结果页面
            getMainWindow()->setCentralWidget(folder);
            currentPageName = PAGE_FOLDER;

            return;
        }
    }

    // 恢复鼠标指针
    QApplication::restoreOverrideCursor();

    // 没有缓存
    // 主窗口切换进入分析界面
    getMainWindow()->setCentralWidget(new AnalysisWidget(path));
}


string MyUtils::toFixed(double num, int fixedSize){
    stringstream ss;
    ss << fixed << setprecision(fixedSize) << num;
    ss >> num;
    return ss.str();
}


string MyUtils::formatSize(qint64 size){
    if (size == 0) {
        return "0";
    }

    double tempSize = size;

    for (int i = 0; ; i++) {
        if (tempSize > 1024) {
            tempSize = tempSize / (double)(1024);
        } else {
            string a = toFixed(tempSize, 0);
            string b = toFixed(tempSize, 1);
            if (b.data()[b.size() - 1] == '0') {
                return a + " " + unitArray[i];
            }

            return b + " " + unitArray[i];
        }
    }
}
