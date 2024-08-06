#ifndef MYUTILS_H
#define MYUTILS_H

#include <QString>
#include <qtypes.h>

using namespace std;

const string unitArray[5] = {"B", "KB", "MB", "GB", "TB"};

/**
 * @brief The MyUtils class
 * 工具类
 */
class MyUtils{
public:
    static void checkCache(QString path);

    /**
     * @brief toFixed 限制小数点后几位
     * @param num
     * @param fixedSize
     * @return
     */
    static string toFixed(double num, int fixedSize);

    /**
     * @brief formatSize 格式化磁盘大小
     * @param size
     * @return
     */
    static string formatSize(qint64 size);

};


#endif
