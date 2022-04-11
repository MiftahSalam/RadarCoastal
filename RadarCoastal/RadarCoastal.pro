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
    ppi/ppievent.cpp \
    framecontrol3.cpp \
    framecursor.cpp \
    frametrail.cpp

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
    ppi/ppievent.h \
    framecontrol3.h \
    framecursor.h \
    frametrail.h

FORMS += \
        mainwindow.ui \
    framecontrol1.ui \
    framecontrol2.ui \
    frameosd.ui \
    frametrackdisplay.ui \
    framecontrol3.ui \
    framecursor.ui \
    frametrail.ui

unix: {
    LIBS += -L/usr/lib/RadarConfig -lRadarConfig
    LIBS += -L/usr/lib/RadarEngine -lRadarEngine

    INCLUDEPATH += /usr/include/RadarConfig
    DEPENDPATH += /usr/include/RadarConfig

    INCLUDEPATH += /usr/include/RadarEngine
    DEPENDPATH += /usr/include/RadarEngine

} else:win32 {
    LIBS += -lOpenGL32

    INCLUDEPATH += C:\Users\miftah\RadarConfigLib\include
    DEPENDPATH += C:\Users\miftah\RadarConfigLib\lib\include

    INCLUDEPATH += C:\Users\miftah\RadarEngineLib\include
    DEPENDPATH += C:\Users\miftah\RadarEngineLib\lib\include
}

#LIBS += -LC:\Users\miftah\RadarConfigLib\lib\ -lRadarConfigd

win32:CONFIG(release, debug|release): LIBS += -LC:\Users\miftah\RadarConfigLib\lib\ -lRadarConfig
else:win32:CONFIG(debug, debug|release): LIBS += -LC:\Users\miftah\RadarConfigLib\lib\ -lRadarConfigd

win32:CONFIG(release, debug|release): LIBS += -LC:\Users\miftah\RadarEngineLib\lib\ -lRadarEngine
else:win32:CONFIG(debug, debug|release): LIBS += -LC:\Users\miftah\RadarEngineLib\lib\ -lRadarEngined


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#DISTFILES += \
#    HMI_style.css

RESOURCES += \
    assets.qrc
