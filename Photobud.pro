QT += widgets
QT += core gui widgets multimedia multimediawidgets

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
main.cpp \
mainwindow.cpp \
menupage.cpp \
camerapage.cpp \
stickerpage.cpp

HEADERS += \
mainwindow.h \
menupage.h \
camerapage.h \
stickerpage.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



# For Windows icon/version info (optional)
# RC_ICONS = resources/icon.ico
