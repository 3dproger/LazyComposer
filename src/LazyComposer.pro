#-------------------------------------------------
#
# Project created by QtCreator 2019-02-24T16:07:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LazyComposer
TEMPLATE = app

TRANSLATIONS += \
    LazyComposer_ru_RU.ts

include(QMidi/src/QMidi.pri)

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

RC_FILE = icon.rc

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        midi.cpp \
        titlegenerator.cpp \
        midistorage.cpp \
        utils.cpp \
        midiplayer.cpp \
        composer.cpp \
        windowmidistructure.cpp \
        patternstorage.cpp \
        translator.cpp

HEADERS += \
        mainwindow.h \
    midi.h \
    global.h \
    titlegenerator.h \
    midistorage.h \
    utils.h \
    midiplayer.h \
    composer.h \
    windowmidistructure.h \
    patternstorage.h \
    translator.h \
    defaults.h

FORMS += \
        mainwindow.ui \
        windowmidistructure.ui


DISTFILES += \
    LazyComposer_ru_RU.ts

RESOURCES += \
    resources.qrc \
    translations.qrc

win32: {
    #Подключаем SSL для Windows. Соответствующий модуль должен быть установлён!!!

    contains(QT_ARCH, i386) {
        #Для Windows x32
        INCLUDEPATH += $$(QTDIR)/../../Tools/OpenSSL/Win_x86/include
    } else {
        #Для Windows x64
        INCLUDEPATH += $$(QTDIR)/../../Tools/OpenSSL/Win_x64/include
    }


    #Сборка файлов релизной версии

    CONFIG(debug, debug|release) {
        #debug
    } else {
        #release
        contains(QT_ARCH, i386) {
            #Для Windows x32
            DESTDIR = $$_PRO_FILE_PWD_/../release_win32
        } else {
            #Для Windows x64
            DESTDIR = $$_PRO_FILE_PWD_/../release_win64
        }

        QMAKE_POST_LINK += $$(QTDIR)\bin\windeployqt --release --qmldir $$(QTDIR)\qml $$DESTDIR $$escape_expand(\\n\\t)
    }
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
