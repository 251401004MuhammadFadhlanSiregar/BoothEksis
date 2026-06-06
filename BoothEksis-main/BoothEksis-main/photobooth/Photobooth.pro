CONFIG += c++17
QT += core gui widgets multimedia multimediawidgets printsupport

TARGET   = SpaceAdventurePhotobooth
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/menupage.cpp \
    src/camerapage.cpp \
    src/stickerpage.cpp \
    src/settingsdialog.cpp

HEADERS += \
    src/mainwindow.h \
    src/menupage.h \
    src/camerapage.h \
    src/stickerpage.h \
    src/settingsdialog.h
