#ifndef FILEINFO_H
#define FILEINFO_H

#include<QString>

class FileInfo{
public:
    QString fileName;
    QString filePath;
    long long size;
    bool isDir;
    QString updateTime;

    long long historySize = 0;
    QString desc = "";

    FileInfo(QString fileName, QString filePath, long long size, bool isDir, QString updateTime){
        this->fileName = fileName;
        this->filePath = filePath;
        this->size = size;
        this->isDir = isDir;
        this->updateTime = updateTime;
    }
};

#endif // FILEINFO_H
