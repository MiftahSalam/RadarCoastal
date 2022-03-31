#-------------------------------------------------
#
# Project created by QtCreator 2022-03-31T15:41:56
#
#-------------------------------------------------

QT       += core gui opengl network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RadarCoastal
TEMPLATE = app

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

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    framecontrol1.cpp

HEADERS += \
        mainwindow.h \
    framecontrol1.h

FORMS += \
        mainwindow.ui \
    framecontrol1.ui

#win32:CONFIG(release, debug|release): LIBS += -LC:\Users\miftah\RadarConfigLib\lib\ -lRadarConfig
#else:win32:CONFIG(debug, debug|release): LIBS += -LC:\Users\miftah\RadarConfigLib\lib\ -lRadarConfigd

LIBS += -LC:\Users\miftah\RadarConfigLib\lib\ -lRadarConfigd

INCLUDEPATH += C:\Users\miftah\RadarConfigLib\include
DEPENDPATH += C:\Users\miftah\RadarConfigLib\lib\include


win32:CONFIG(release, debug|release): LIBS += -LC:\Users\miftah\RadarEngineLib\lib\ -lRadarEngine
else:win32:CONFIG(debug, debug|release): LIBS += -LC:\Users\miftah\RadarEngineLib\lib\ -lRadarEngined

INCLUDEPATH += C:\Users\miftah\RadarEngineLib\include
DEPENDPATH += C:\Users\miftah\RadarEngineLib\lib\include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
