#ifndef ANALYSIS_HISTORY_WIDGET_H
#define ANALYSIS_HISTORY_WIDGET_H

#include <QWidget>
#include <QFileInfo>

namespace Ui {
class AnalysisHistoryWidget;
}

class AnalysisHistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisHistoryWidget(QWidget *parent = nullptr, QString diskSymbol = "");
    ~AnalysisHistoryWidget();

signals:
    void selectHistoryFileSignal();

private slots:
    void on_pushButton_2_clicked();

private:
    Ui::AnalysisHistoryWidget *ui;
    // 当前所处的盘符
    QString diskSymbol;

    QList<QFileInfo> getHistoryResultFiles();
};

#endif // ANALYSIS_HISTORY_WIDGET_H
