QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GUI/mainwindow.cpp \
    logger/crashhandler.cpp \
    logger/logger.cpp \
    logger/logging_categories.cpp \
    logger/logworker.cpp \
    main.cpp \

HEADERS += \
    GUI/mainwindow.h \
    logger/crashhandler.h \
    logger/logger.h \
    logger/logging_categories.h \
    logger/logworker.h \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
