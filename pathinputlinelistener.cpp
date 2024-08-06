#include "pathinputlinelistener.h"
#include "utils.h"

#include <QEvent>
#include <QKeyEvent>

PathInputLineListener::PathInputLineListener(QLineEdit *watchedItem) {
    this->watchedItem = watchedItem;
}

bool PathInputLineListener::eventFilter(QObject *watched, QEvent *event){
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            QString path = watchedItem->text().replace("\\", "/");
            // 如果地址不以 "/" 结尾,手动加上
            if(!path.endsWith("/")){
                path += "/";
            }

            // 处理回车事件
            MyUtils::checkCache(path);

            // 如果不希望继续传递事件，返回true；否则返回false
            return true;
        }
    }

    return false;
}
