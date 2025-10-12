#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStorageInfo>
#include <QProgressBar>
#include "home_widget.h"
#include <QMenuBar>
#include <QGuiApplication>
#include <QStyleHints>
#include "globalvariable.h"

#include<QJsonObject>
#include<QJsonDocument>
#include<QNetworkAccessManager>
#include<QNetworkRequest>
#include<QNetworkReply>
#include<QRegularExpression>
#include<QHostInfo>
#include<QDate>
#include<QDesktopServices>
#include<QUrl>
#include<QDialog>
#include<QTextEdit>
#include<QDialogButtonBox>
#include<QPushButton>

using namespace std;

// 主窗口
//static QMainWindow *mainWindow;
// 程序首页
//static QWidget *homeWidget;

MainWidget::MainWidget(QMainWindow *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    this->setWindowTitle("Disk Analysis Tool v" + QString(CURRENT_VERSION));

    // 适配深色模式
    if(QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark){
        this->setStyleSheet("QWidget { background-color:black; }");
        setIsSystemDarkMode(true);
    }else{
        this->setStyleSheet("QWidget { background-color:white; }");
        setIsSystemDarkMode(false);
    }
    // 监听系统模式切换
    connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
            this, [=](Qt::ColorScheme newColorScheme){
        if (newColorScheme == Qt::ColorScheme::Dark){
            this->setStyleSheet("QWidget { background-color:black; }");
            setIsSystemDarkMode(true);
        }else{
            this->setStyleSheet("QWidget { background-color:white; }");
            setIsSystemDarkMode(false);
        }

        // 重绘界面
        repaintCentralWidget();
    });


    // 设置最大线程数为cpu核心线程数减一
    int coreThreadSize = std::thread::hardware_concurrency();

    // qDebug("cpu逻辑核心数: %d", coreThreadSize);

    getThreadPool()->setMaxThreadCount(coreThreadSize > 6 ?  coreThreadSize - 1 : 6);

    // 创建首页界面
    QWidget *centralWidget = new HomeWidget();

    // 设置中心组件
    this->setCentralWidget(centralWidget);
    currentPageName = PAGE_HOME;

    // this为主窗口
    //mainWindow = this;
    setMainWindow(this);

    // 当前exe程序所在路径
    QString exePath = QCoreApplication::applicationDirPath();
    // 发送使用记录, 开发环境不发送
    if(exePath.contains("build") && (exePath.contains("release") || exePath.contains("debug"))){
        qDebug() << "当前为开发环境, 不发送使用统计";
    }else{
        sendUsageCount();
    }
    // 检查更新
    checkUpdate();

}

MainWidget::~MainWidget()
{
    delete ui;

}

// 获取免费api LeanCloud 的访问域名
void MainWidget::getApiHost(bool isSendUsage){
    QNetworkAccessManager *manager = new QNetworkAccessManager();

    // 设置请求 URL
    QUrl url("https://wenku.baidu.com/wpeditor/getdocument?axios_original=1");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // 构造 JSON 数据
    QJsonObject json;
    json["fsid"] = 1101408526343678;
    json["scene"] = "fcbshare";
    json["uk"] = 556868662;
    json["shareid"] = "5xkPedtvDL88S2vwJnIoPvQItKbzJWDVtUyeJN4Kl7g";

    QByteArray data = QJsonDocument(json).toJson();

    // 发送 POST 请求
    QNetworkReply *reply = manager->post(request, data);

    // 连接信号槽处理响应
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QString responseStr = QString(response);
            // 使用正则匹配出域名
            QRegularExpression hostReg(R"((?<=(LeanCloud_host=))[^\"]+)");
            QRegularExpressionMatch matchRes = hostReg.match(response);
            if(matchRes.hasMatch()){
                QString host = matchRes.captured(0);
                if(!host.isEmpty()){
                    qDebug() << "从百度笔记中读取到api host: " << host;
                    if(isSendUsage){
                        this->sendUsageCount(host);
                    }else{
                        this->checkUpdate(host);
                    }
                }
            }
        } else {
            qDebug() << "Http Request Error:" << reply->errorString();
        }
        reply->deleteLater();
        manager->deleteLater();
    });

}


QString lastInvalidApiHost = ""; //上一次使用的无效api域名

// 发送软件使用统计
void MainWidget::sendUsageCount(QString apiHost){
    // 设置默认的api域名
    if(apiHost.isEmpty()){
        apiHost = DEFAULT_API_HOST;
    }
    // 当前域名无效
    if(apiHost == lastInvalidApiHost){
        return;
    }

    // 发送软件使用情况
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    // 设置请求 URL
    QUrl url(apiHost + "/1.1/classes/disk_analysis_tool_usage_count");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-LC-Id", X_LC_Id);
    request.setRawHeader("X-LC-Key", X_LC_Key);

    // 获取主机名
    QString username = QHostInfo::localHostName();

    // 构造 JSON 数据
    QJsonObject json;
    json["username"] = username.isEmpty() ? "unknown" : username;
    json["date"] = QDate::currentDate().toString("yyyyMMdd");
    json["version"] = CURRENT_VERSION;

    QByteArray data = QJsonDocument(json).toJson();
    // 发送 POST 请求
    QNetworkReply *reply = manager->post(request, data);

    // 连接信号槽处理响应
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QString responseStr = QString(response);
            if(responseStr.contains("objectId")){
                qDebug() << "发送使用统计成功";
            }
        } else {
            qDebug() << "Http Request Error:" << reply->errorString();
            lastInvalidApiHost = apiHost;
            getApiHost();
        }
        reply->deleteLater();
        manager->deleteLater();
    });
}

// 检查软件更新
void MainWidget::checkUpdate(QString apiHost){
    // 设置默认的api域名
    if(apiHost.isEmpty()){
        apiHost = DEFAULT_API_HOST;
    }
    // 当前域名无效
    if(apiHost == lastInvalidApiHost){
        return;
    }

    // 检查软件更新
    QNetworkAccessManager *manager2 = new QNetworkAccessManager();
    // 设置请求 URL
    QUrl url2(apiHost + "/1.1/classes/disk_analysis_tool_update/68eb30862f7ee809fcdbf774");
    QNetworkRequest request2(url2);
    request2.setRawHeader("X-LC-Id", X_LC_Id);
    request2.setRawHeader("X-LC-Key", X_LC_Key);

    // 发送 GET 请求
    QNetworkReply *reply2 = manager2->get(request2);

    // 连接信号槽处理响应
    QObject::connect(reply2, &QNetworkReply::finished, [=]() {
        if (reply2->error() == QNetworkReply::NoError) {
            QByteArray response = reply2->readAll();
            QString responseStr = QString(response);
            if(responseStr.contains("objectId")){
                QJsonDocument doc = QJsonDocument::fromJson(response);
                if(!doc.isNull() && doc.isObject()){
                    auto obj = doc.object();
                    // 获取最新版本号, 跟目前版本号比较
                    if(obj.contains("latest_version") && obj["latest_version"].toString() > CURRENT_VERSION){
                        qDebug() << "检查到有新版本";


                        QString updateDesc = (obj.contains("desc") && !obj["desc"].toString().isEmpty() ? obj["desc"].toString() : "暂无更新日志");
                        updateDesc.replace("\n", "<br>");
                        updateDesc.replace(" ", "&nbsp;");

                        QString text = "<b style='color:green;font-size:14px;'>新版本: v" + obj["latest_version"].toString() + "</b>"
                                       + "<br><br>"
                                       + "<b>更新内容</b>:<br>"
                                       + updateDesc;

                        QDialog dialog;
                        dialog.setWindowTitle("版本更新提醒");
                        dialog.setMinimumSize(500, 400);
                        QVBoxLayout *layout = new QVBoxLayout(&dialog);

                        QTextEdit *textEdit = new QTextEdit();
                        textEdit->setText(text);
                        textEdit->setReadOnly(true);

                        QDialogButtonBox *buttonBox = new QDialogButtonBox();

                        // 添加github下载按钮
                        QPushButton *githubButton = new QPushButton("github下载");
                        buttonBox->addButton(githubButton, QDialogButtonBox::ActionRole);
                        connect(githubButton, &QPushButton::clicked, [&](){
                            QDesktopServices::openUrl(QUrl(obj.contains("download_github") && !obj["download_github"].toString().isEmpty() ? obj["download_github"].toString() : ""));
                        });

                        // 添加蓝奏云下载按钮
                        QPushButton *lanzouButton = new QPushButton("蓝奏云下载");
                        buttonBox->addButton(lanzouButton, QDialogButtonBox::ActionRole);
                        connect(lanzouButton, &QPushButton::clicked, [&](){
                            QDesktopServices::openUrl(QUrl(obj.contains("download_lanzou") && !obj["download_lanzou"].toString().isEmpty() ? obj["download_lanzou"].toString() : ""));
                        });

                        // 添加取消按钮
                        QPushButton *cancelButton = new QPushButton("取消");
                        buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);
                        connect(cancelButton, &QPushButton::clicked, [&](){
                            dialog.close();
                        });

                        layout->addWidget(textEdit);
                        layout->addWidget(buttonBox);

                        dialog.exec();
                    }
                }
            }
        } else {
            qDebug() << "Http Request Error:" << reply2->errorString();
            //QMessageBox::critical(parseWarningLog(QString("<b>检查版本更新</b>: api域名").append("[").append(apiHost).append("]访问失败, 错误信息: ").append(reply2->errorString())));

            lastInvalidApiHost = apiHost;
            getApiHost(false);
        }
        reply2->deleteLater();
        manager2->deleteLater();
    });
}





