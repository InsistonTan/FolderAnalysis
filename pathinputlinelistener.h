#ifndef PATHINPUTLINELISTENER_H
#define PATHINPUTLINELISTENER_H

#include <QObject>
#include <QLineEdit>

class PathInputLineListener : public QObject{
    Q_OBJECT
private:
    QLineEdit *watchedItem;
public:
    PathInputLineListener(QLineEdit *watchedItem);
    ~PathInputLineListener(){}
    //bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
    void editLineSlot();
};

#endif // PATHINPUTLINELISTENER_H
