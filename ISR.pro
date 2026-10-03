QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


# =========================
# Dependencies
# =========================

SDK_ROOT = $$(JOBQT_SDK)

isEmpty(SDK_ROOT) {
    error("JOBQT_SDK environment variable is not set")
}

LOGGER_VERSION = 1.0.0
ENCODINGDETECTOR_VERSION = 1.0.0

LOGGER_ROOT = $$SDK_ROOT/Logger/$$LOGGER_VERSION
ENCODINGDETECTOR_ROOT = $$SDK_ROOT/EncodingDetector/$$ENCODINGDETECTOR_VERSION

!exists($$LOGGER_ROOT) {
    error("Logger $$LOGGER_VERSION not found: $$LOGGER_ROOT")
}
!exists($$ENCODINGDETECTOR_ROOT) {
    error("EncodingDetector $$ENCODINGDETECTOR_VERSION not found: $$ENCODINGDETECTOR_ROOT")
}

INCLUDEPATH += \
    $$LOGGER_ROOT/include \
    $$ENCODINGDETECTOR_ROOT/include

LIBS += \
    -L$$LOGGER_ROOT/lib \
    -lLogger \
    -L$$ENCODINGDETECTOR_ROOT/lib \
    -lEncodingDetector


# =========================
# Sources
# =========================

SOURCES += \
    Services/Application/application.cpp \
    Services/CatalogManager/catalogmanager.cpp \
    Services/ConfigReader/configparser.cpp \
    Services/ConfigReader/configreader.cpp \
    Services/ConfigReader/configschema.cpp \
    Services/Controller/controller.cpp \
    Services/DirectiveExecutor/directiveexecutor.cpp \
    Services/Directives/commanddirective.cpp \
    Services/Directives/dokladdirective.cpp \
    Services/Directives/insructionsdirective.cpp \
    Services/Directives/mainoperdirecitve.cpp \
    Services/Directives/startsectiondirective.cpp \
    Services/Directives/variantdirective.cpp \
    Services/FilesReaders/diireader.cpp \
    Services/FilesReaders/sectionsloader.cpp \
    Services/FilesReaders/setfilesreader.cpp \
    Services/GUI/MenuDialogWindow/TrafaretDialog/trafaretdialog.cpp \
    Services/GUI/MenuDialogWindow/TrafaretDialog/trafaretlineedit.cpp \
    Services/GUI/MenuDialogWindow/controlspodialog.cpp \
    Services/GUI/MenuDialogWindow/finddialog.cpp \
    Services/GUI/MenuDialogWindow/listofsections.cpp \
    Services/GUI/MenuDialogWindow/sectionlistdialog.cpp \
    Services/GUI/MenuDialogWindow/titleofsectiondialog.cpp \
    Services/GUI/ProgViewWgt/programtablemodel.cpp \
    Services/GUI/ProgViewWgt/programviewwidget.cpp \
    Services/GUI/mainwindow.cpp \
    logging_categories.cpp \
    main.cpp

HEADERS += \
    Services/Application/application.h \
    Services/CatalogManager/catalogmanager.h \
    Services/ConfigReader/configparser.h \
    Services/ConfigReader/configreader.h \
    Services/ConfigReader/configschema.h \
    Services/ConfigReader/configtypes.h \
    Services/Controller/appstate.h \
    Services/Controller/controller.h \
    Services/DirectiveExecutor/directiveexecutor.h \
    Services/DirectiveExecutor/executiontypes.h \
    Services/Directives/commanddirective.h \
    Services/Directives/directive.h \
    Services/Directives/directivetypes.h \
    Services/Directives/dokladdirective.h \
    Services/Directives/insructionsdirective.h \
    Services/Directives/mainoperdirecitve.h \
    Services/Directives/startsectiondirective.h \
    Services/Directives/variantdirective.h \
    Services/FilesReaders/ErrorReadFileStruct.h \
    Services/FilesReaders/diireader.h \
    Services/FilesReaders/sectionsloader.h \
    Services/FilesReaders/setfilesreader.h \
    Services/GUI/MenuDialogWindow/TrafaretDialog/trafaretdialog.h \
    Services/GUI/MenuDialogWindow/TrafaretDialog/trafaretlineedit.h \
    Services/GUI/MenuDialogWindow/controlspodialog.h \
    Services/GUI/MenuDialogWindow/finddialog.h \
    Services/GUI/MenuDialogWindow/listofsections.h \
    Services/GUI/MenuDialogWindow/sectionlistdialog.h \
    Services/GUI/MenuDialogWindow/titleofsectiondialog.h \
    Services/GUI/ProgViewWgt/programtablemodel.h \
    Services/GUI/ProgViewWgt/programviewwidget.h \
    Services/GUI/guitypes.h \
    Services/GUI/mainwindow.h \
    Services/PRIS/pristypes.h \
    Services/Programs/programlines.h \
    Services/Programs/programtypes.h \
    Services/RequestIDGenerator/requestIDGenerator.h \
    logging_categories.h


# =========================
# Deployment
# =========================

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin

!isEmpty(target.path): INSTALLS += target


DISTFILES += \
    Services/ConfigReader/readme.md
