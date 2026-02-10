QT += core gui widgets network

CONFIG += c++17

SOURCES += \
    main.cpp \
    src/core/appconfig.cpp \
    src/core/configloader.cpp \
    src/core/isrcontroller.cpp \
    src/core/sporepository.cpp \
    src/net/pormessage.cpp \
    src/net/pristransport.cpp \
    src/services/directivehandler.cpp \
    src/services/protocollogger.cpp \
    src/ui/headerdialog.cpp \
    src/ui/mainwindow.cpp \
    src/ui/sectionlistdialog.cpp \
    src/ui/stencildialog.cpp

HEADERS += \
    src/core/appconfig.h \
    src/core/configloader.h \
    src/core/isrcontroller.h \
    src/core/sporepository.h \
    src/net/pormessage.h \
    src/net/pristransport.h \
    src/services/directivehandler.h \
    src/services/protocollogger.h \
    src/ui/headerdialog.h \
    src/ui/mainwindow.h \
    src/ui/sectionlistdialog.h \
    src/ui/stencildialog.h
