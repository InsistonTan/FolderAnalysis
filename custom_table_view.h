#ifndef CUSTOM_TABLE_VIEW_H
#define CUSTOM_TABLE_VIEW_H

#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QTableView>
#include <QGraphicsDropShadowEffect>
#include "fileinfo.h"
#include <QProgressDialog>
#include <QFile>
#include <QDir>
#include "globalvariable.h"
#include <QApplication>

class CustomTableView : public QTableView
{
    Q_OBJECT
private:
    QList<FileInfo *> resultList;
public:
    CustomTableView(QList<FileInfo *> &resultList, QWidget *parent = nullptr) : QTableView(parent){
        this->resultList = resultList;
    }

protected:
    void contextMenuEvent(QContextMenuEvent *event) override
    {
        // 选中的行不为空
        if (!this->selectionModel()->selectedRows().isEmpty()) {
            // 创建右键菜单
            QMenu menu(this);
            menu.setStyleSheet(
                "QMenu { "
                "   background-color: rgb(252, 252, 252); "
                "   padding: 2px;"
                "   border-radius:5px; "
                "   border: 2px solid rgb(237, 237, 237);"
                " }"
                "QMenu::item { padding: 8 50 8 0; }"  // 设置菜单项的内边距
                "QMenu::item:selected { background-color: rgb(243, 243, 243); color: black; }"  // 鼠标悬停时的选中项
                "QMenu::item:disabled { color: gray; }"  // 禁用项的颜色
                "QAction { font-size: 14px; color: black; }");  // 设置菜单项字体颜色)

            // 添加菜单项
            QAction *deleteAction = new QAction("删除", this);
            // 设置图标
            deleteAction->setIcon(QIcon(":/res/static/icon_delete.png"));
            // 绑定事件和槽
            connect(deleteAction, &QAction::triggered, this, &CustomTableView::deleteRow);

            // 添加菜单项到菜单
            menu.addAction(deleteAction);

            // 显示菜单
            menu.exec(event->globalPos());
        }
    }

    QList<FileInfo *> deleteFilesWithProgress(const QList<FileInfo *> fileList) {
        // 实际已经被删除的文件列表
        QList<FileInfo *> deleteFileListSuccess;

        // 创建一个进度对话框
        QProgressDialog progress("正在删除文件/文件夹: ", "取消", 0, fileList.size() * 100 + 1);
        progress.setWindowModality(Qt::WindowModal); // 确保进度框是模态的，阻止与其他窗口交互
        progress.setMinimumDuration(1000); // 设置最小显示时长，避免进度框太快消失
        progress.setValue(0);
        progress.show(); // 显式显示进度对话框

        // 遍历文件列表，删除每个文件
        for (int i = 0; i < fileList.size(); ++i) {
            // 检查是否取消操作
            if (progress.wasCanceled()) {
                QMessageBox::information(nullptr, "操作取消", "删除操作已被取消.");
                return deleteFileListSuccess;
            }

            // 更新正在删除的文件/文件夹
            progress.setLabelText(fileList[i]->isDir ? "正在删除文件夹: " : "正在删除文件: " + fileList[i]->fileName);

            // 强制更新进度对话框
            QApplication::processEvents();

            QString filePath = fileList[i]->filePath;

            QFileInfo fileInfo(filePath);
            if (!fileInfo.isWritable()) {
                QMessageBox::information(nullptr, "提示", "没有权限删除文件/文件夹: " + filePath);
                continue;
            }

            QFile file(filePath);

            if (file.exists() && fileInfo.isFile()){
                // 删除文件失败
                if(!file.remove()) {
                    QMessageBox::warning(nullptr, "删除失败", QString("无法删除文件: %1").arg(filePath));
                }else{
                    deleteFileListSuccess.append(fileList[i]);

                }
            }

            // 删除文件夹（递归删除）
            if (QDir(filePath).exists()) {
                QDir dir(filePath);
                // 删除失败
                if(!dir.removeRecursively()){
                    QMessageBox::warning(nullptr, "删除失败", QString("无法删除文件夹: %1").arg(filePath));
                }else{
                    deleteFileListSuccess.append(fileList[i]);
                }
            }


            // 更新进度
            progress.setValue((i+1) * 100);

            // 处理事件，确保UI更新
            QApplication::processEvents();

            // 添加小延迟，确保能看到进度变化
            QThread::msleep(300/fileList.size());
        }

        QThread::msleep(500);

        // 设置进度为完成
        progress.setValue(fileList.size() * 100 + 1);

        // 处理事件，确保UI更新
        QApplication::processEvents();


        // 完成删除后，显示一个信息框
        QMessageBox::information(nullptr, "完成", "文件删除完成");

        return deleteFileListSuccess;
    }

    bool isCurrentRowFileInList(QList<FileInfo *> fileList, int currentRowIndex){
        if(fileList.isEmpty() || currentRowIndex < 0){
            return false;
        }

        for(auto item : fileList){
            if(item->fileName == resultList[currentRowIndex]->fileName){
                return true;
            }
        }

        return false;
    }

    // 获取上一级目录
    QString getParentPath(QString currentPath){
        QString result;
        if(currentPath.isEmpty()){
            return result;
        }

        QStringList strList = currentPath.split("/");
        int endIndex = currentPath.endsWith("/") ? strList.size() - 2 : strList.size() - 1;
        for(int i = 0; i < endIndex; i++){
            result.append(strList[i]).append("/");
        }

        return result;
    }

    // 更新缓存
    void updateCache(QList<FileInfo *> successDeleteFileList){
        if(successDeleteFileList.isEmpty()){
            return;
        }

        for(auto deletedFileInfo : successDeleteFileList){
            // 文件绝对路径
            auto filePath = deletedFileInfo->filePath;

            // 截取出盘符
            auto diskSymbol = filePath.left(3);

            // 检查缓存, 该盘存在缓存
            if(result_cache.contains(diskSymbol)){
                // 取出缓存
                auto cache = getValueFromCache(diskSymbol);

                // 将文件路径分离
                auto filePathSplitList = filePath.split("/");

                // 将已删除的文件从缓存中移除
                if(cache.contains(filePath)){
                    cache.remove(filePath);
                }

                // 父级目录绝对路径
                QString parentDir = "";

                // 更新该文件的所有父级目录的大小
                for(auto dir : filePathSplitList){
                    parentDir += dir + "/";

                    // 更新该父级的总大小
                    if(cache.contains(parentDir)){
                        cache[parentDir]->size -= deletedFileInfo->size;
                    }
                }

                // 更新修改后的缓存
                updateResultCache(diskSymbol, cache);
            }
        }

    }
private slots:
    void deleteRow()
    {
        // 获取选中的行
        QModelIndexList selectedIndexes = this->selectionModel()->selectedRows();

        if (selectedIndexes.isEmpty()) {
            // 如果没有选中任何行，弹出警告
            QMessageBox::warning(this, "没有选择项目", "请选择一个项目删除");
        } else {
            // 待删除的文件列表
            QList<FileInfo *> deleteFileList;
            // 文件列表文本信息
            QString fileMsg;
            for (const QModelIndex &index : selectedIndexes) {
                int row = index.row();

                deleteFileList.append(resultList[row]);
                fileMsg.append(resultList[row]->isDir ? "[文件夹] " : "[文件] ").append(resultList[row]->fileName).append("\n");
            }

            // 创建一个 QMessageBox 对象，带有 OK 和 Cancel 按钮
            QMessageBox msgBox;
            msgBox.setWindowTitle("提醒");
            msgBox.setText("请确认删除以下文件/文件夹:\t\n\n" + fileMsg);
            // 设置图标
            msgBox.setIcon(QMessageBox::Warning);
            // 创建并添加自定义按钮
            QPushButton* ok = msgBox.addButton("确认", QMessageBox::ActionRole);
            QPushButton* cancel = msgBox.addButton("取消", QMessageBox::RejectRole);

            // 设置默认按钮
            msgBox.setDefaultButton(ok);

            // 显示消息框
            msgBox.exec();

            // 判断用户点击确认按钮
            if ((QPushButton*)msgBox.clickedButton() == ok) {
                // 删除实际的文件
                QList<FileInfo *> successDeleteFileList = deleteFilesWithProgress(deleteFileList);
                if(successDeleteFileList.isEmpty()){
                    return;
                }

                // 将行号倒序
                QList<int> rowIndexList;
                for(auto item : selectedIndexes){
                    rowIndexList.append(item.row());
                }
                // 重新排序
                std::sort(rowIndexList.begin(), rowIndexList.end(), [](int f1, int f2){
                    return f1 > f2;
                });



                // 倒序删除界面上的选中的行(因为删除时行号会变化, 倒着删除可以保证前面的行号不变)
                for (int row : rowIndexList) {
                    if(isCurrentRowFileInList(successDeleteFileList, row)){
                        this->model()->removeRow(row);  // 界面上删除该行

                        // 临时结果也删除
                        resultList.removeAt(row);
                    }
                }

                // 更新缓存
                updateCache(successDeleteFileList);
            }
        }
    }
};
#endif // CUSTOM_TABLE_VIEW_H
