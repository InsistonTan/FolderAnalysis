#include "utils.h"
#include <fileinfo.h>
#include "folder_widget.h"
#include "analysis_widget.h"
#include <QMainWindow>
#include <iomanip>
#include <sstream>
#include "globalvariable.h"

// 引入全局变量
// 结果缓存
//extern QMap<QString, QList<FileInfo *>> RESULT_CACHE;
// 主窗口
//extern QMainWindow *mainWin;

void MyUtils::checkCache(QString path){
    //qDebug() << getResultCache().values();


    // 检查缓存,如果有缓存就直接进入结果页面
    QList<FileInfo *> cacheList = getResultCache().value(path);
    if(!cacheList.empty()){
        // 直接进入结果页面
        getMainWindow()->setCentralWidget(new FolderWidget(path, cacheList));
        currentPageName = PAGE_FOLDER;
    }else{
        // 主窗口切换进入分析界面
        getMainWindow()->setCentralWidget(new AnalysisWidget(path));
    }
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
