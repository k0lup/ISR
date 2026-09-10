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
    GUI/mainwindow.cpp \
    Services/ConfigReader/configparser.cpp \
    Services/ConfigReader/configreader.cpp \
    Services/ConfigReader/configschema.cpp \
    Services/DirectiveExecutor/directiveexecutor.cpp \
    logging_categories.cpp \
    main.cpp

HEADERS += \
    GUI/mainwindow.h \
    Services/ConfigReader/configparser.h \
    Services/ConfigReader/configreader.h \
    Services/ConfigReader/configschema.h \
    Services/ConfigReader/configtypes.h \
    Services/DirectiveExecutor/directiveexecutor.h \
    Services/DirectiveExecutor/program.h \
    logging_categories.h


# =========================
# Deployment
# =========================

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin

!isEmpty(target.path): INSTALLS += target


DISTFILES += \
    Services/ConfigReader/readme.md
