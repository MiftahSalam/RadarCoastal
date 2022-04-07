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

include(stream/qmqtt/qmqtt.pri)

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    framecontrol1.cpp \
    framecontrol2.cpp \
    frameosd.cpp \
    stream/navsensor.cpp \
    stream/stream.cpp \
    stream/mqttdevicewrapper.cpp \
    stream/devicewrapper.cpp \
    frametrackdisplay.cpp \
    ppi/radarwidget.cpp \
    ppi/ppievent.cpp

HEADERS += \
        mainwindow.h \
    framecontrol1.h \
    framecontrol2.h \
    frameosd.h \
    stream/navsensor.h \
    stream/stream.h \
    stream/mqttdevicewrapper.h \
    stream/devicewrapper.h \
    frametrackdisplay.h \
    ppi/radarwidget.h \
    ppi/ppievent.h

FORMS += \
        mainwindow.ui \
    framecontrol1.ui \
    framecontrol2.ui \
    frameosd.ui \
    frametrackdisplay.ui

#win32:CONFIG(release, debug|release): LIBS += -LC:\Users\miftah\RadarConfigLib\lib\ -lRadarConfig
#else:win32:CONFIG(debug, debug|release): LIBS += -LC:\Users\miftah\RadarConfigLib\lib\ -lRadarConfigd

LIBS += -LC:\Users\miftah\RadarConfigLib\lib\ -lRadarConfigd

INCLUDEPATH += C:\Users\miftah\RadarConfigLib\include
DEPENDPATH += C:\Users\miftah\RadarConfigLib\lib\include


win32: LIBS += -lOpenGL32

win32:CONFIG(release, debug|release): LIBS += -LC:\Users\miftah\RadarEngineLib\lib\ -lRadarEngine
else:win32:CONFIG(debug, debug|release): LIBS += -LC:\Users\miftah\RadarEngineLib\lib\ -lRadarEngined

INCLUDEPATH += C:\Users\miftah\RadarEngineLib\include
DEPENDPATH += C:\Users\miftah\RadarEngineLib\lib\include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#DISTFILES += \
#    HMI_style.css

RESOURCES += \
    assets.qrc
