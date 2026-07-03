QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Directives/commanddirective.cpp \
    Directives/dokladdirective.cpp \
    Directives/insructionsdirective.cpp \
    Directives/mainoperdirecitve.cpp \
    config/config_loader.cpp \
    config/config_service.cpp \
    config/startup.cpp \
    gui/diiViewer/diiviewer.cpp \
    gui/isrmainwindow.cpp \
    gui/loadingoverlay.cpp \
    gui/menuWgt/findwgt.cpp \
    gui/menuWgt/sectionlistwgt.cpp \
    gui/menuWgt/titlesectionwgt.cpp \
    logger/crashhandler.cpp \
    logger/logger.cpp \
    logger/logging_categories.cpp \
    logger/logworker.cpp \
    main.cpp \
    servises/catalogmanager.cpp \
    setReader/diireader.cpp \
    setReader/sectionsloader.cpp \
    setReader/setfilesreader.cpp

HEADERS += \
    Directives/commanddirective.h \
    Directives/directive.h \
    Directives/dokladdirective.h \
    Directives/insructionsdirective.h \
    Directives/mainoperdirecitve.h \
    ErrorReadFileStruct.h \
    config/app_config.h \
    config/config_loader.h \
    config/config_service.h \
    config/config_types.h \
    config/startup.h \
    gui/diiViewer/diiviewer.h \
    gui/isrmainwindow.h \
    gui/loadingoverlay.h \
    gui/menuWgt/findwgt.h \
    gui/menuWgt/sectionlistwgt.h \
    gui/menuWgt/titlesectionwgt.h \
    logger/crashhandler.h \
    logger/logger.h \
    logger/logging_categories.h \
    logger/logworker.h \
    servises/catalogmanager.h \
    setReader/diireader.h \
    setReader/sectionsloader.h \
    setReader/setfilesreader.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
