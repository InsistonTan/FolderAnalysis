#ifndef PATHINPUTLINELISTENER_H
#define PATHINPUTLINELISTENER_H

#include <QObject>
#include <QLineEdit>

class PathInputLineListener  : public QObject{
private:
    QLineEdit *watchedItem;
public:
    PathInputLineListener(QLineEdit *watchedItem);
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // PATHINPUTLINELISTENER_H
