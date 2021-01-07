QT       += core gui
TARGET = KpEdit
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/include/opencv4
LIBS += -lopencv_core -lopencv_highgui -lopencv_video -lopencv_videostab -lopencv_videoio
DESTDIR = ../
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    ui/aboutdialog.ui \
    ui/exportdialog.ui \
    ui/kpplayer.ui \
    ui/linkdialog.ui \
    ui/mainwindow.ui \
    ui/renderdialog.ui

HEADERS += \
    src/aboutdialog.h \
    src/exportdialog.h \
    src/interfaces.h \
    src/keypoint.h \
    src/kpplayer.h \
    src/linkdialog.h \
    src/mainwindow.h \
    src/previewwidget.h \
    src/renderdialog.h

SOURCES += \
    src/aboutdialog.cpp \
    src/exportdialog.cpp \
    src/kpplayer.cpp \
    src/linkdialog.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/previewwidget.cpp \
    src/renderdialog.cpp

RESOURCES += \
    res.qrc

DISTFILES += \
    icon.rc
