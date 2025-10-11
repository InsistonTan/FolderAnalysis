QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    analysis_history_widget.cpp \
    analysis_widget.cpp \
    folder_widget.cpp \
    globalvariable.cpp \
    home_widget.cpp \
    main.cpp \
    mainwindow.cpp \
    pathinputlinelistener.cpp \
    utils.cpp


HEADERS += \
    analysis_history_widget.h \
    analysis_thread.h \
    analysis_widget.h \
    analysis_worker.h \
    custom_table_view.h \
    fileinfo.h \
    folder_widget.h \
    globalvariable.h \
    home_widget.h \
    mainwindow.h \
    pathinputlinelistener.h \
    utils.h

FORMS += \
    analysis_history_widget.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    staticResource.qrc

RC_FILE = appicon.rc
