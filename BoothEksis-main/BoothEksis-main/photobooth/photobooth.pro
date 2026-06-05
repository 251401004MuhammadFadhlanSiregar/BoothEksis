CONFIG += c++17
QT += core gui widgets multimedia multimediawidgets
QT += core gui widgets multimedia multimediawidgets printsupport
QT += printsupport

CONFIG += c++17

TARGET   = SpaceAdventurePhotobooth
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/menupage.cpp \
    src/camerapage.cpp \
    src/stickerpage.cpp

HEADERS += \
    src/mainwindow.h \
    src/menupage.h \
    src/camerapage.h \
    src/stickerpage.h

# For Windows icon/version info (optional)
# RC_ICONS = resources/icon.ico
