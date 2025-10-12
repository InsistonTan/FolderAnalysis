#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#define CURRENT_VERSION "1.0.3" // 当前版本号

#define DEFAULT_API_HOST "https://xh36dstw.lc-cn-n1-shared.com" //默认的api域名
#define X_LC_Id "xH36dsTwk1T5XoXmO4EHA1qg-gzGzoHsz"
#define X_LC_Key "z5mrlT4AtWBnPLRFysmGKyNZ"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    MainWidget(QMainWindow *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;

    // 获取免费api LeanCloud 的访问域名
    void getApiHost(bool isSendUsage = true);
    // 发送软件使用统计
    void sendUsageCount(QString apiHost = "");
    // 检查软件是否有更新
    void checkUpdate(QString apiHost = "");
};
#endif // MAINWINDOW_H
