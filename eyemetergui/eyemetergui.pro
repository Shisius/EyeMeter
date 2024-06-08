QT       += core gui
#svg
#QT += virtualkeyboard
QTPLUGINS += qtvirtualkeyboardplugin
#static {
#      QTPLUGIN += qtvirtualkeyboardplugin
#      QT += svg
#  }
#QT_QPA_PLATFORM=wayland
#QT_QPA_PLATFORMTHEME=gnome
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../ipc/shmem_alloc.cpp \
    ../ipc/udsunicomm.cpp \
    dateedit_clickable.cpp \
    dateedit_keyboard.cpp \
    emtoolbar.cpp \
    inputdatedialog.cpp \
    inputnamedialog.cpp \
    lineedit_clickable.cpp \
    lineedit_keyboard.cpp \
    main.cpp \
    mainwindow.cpp \
    udsunisocket.cpp

HEADERS += \
    ../ipc/shmem_alloc.h \
    ../ipc/udsunicomm.h \
    dateedit_clickable.h \
    dateedit_keyboard.h \
    emtoolbar.h \
    inputdatedialog.h \
    inputnamedialog.h \
    lineedit_clickable.h \
    lineedit_keyboard.h \
    mainwindow.h \
    udsunisocket.h
INCLUDEPATH += ../ipc \
               ../types

TRANSLATIONS += \
    eyemetergui_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#DEFINES += TEST_snapshot
