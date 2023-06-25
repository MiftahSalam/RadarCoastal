#-------------------------------------------------
#
# Project created by QtCreator 2022-03-31T15:41:56
#
#-------------------------------------------------

QT       += core gui opengl network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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

#MODE += DEBUG_MODE

equals(MODE,"DEBUG_MODE") {
    TARGET = RadarCoastal-console
    CONFIG += console
}
else {
    TARGET = RadarCoastal
}

include(infra/qmqtt/qmqtt.pri)

SOURCES += \
    usecase/alarm/alarm.cpp \
    usecase/alarm/alarmmanager.cpp \
    usecase/alarm/gzalarm.cpp \
    view/dialog/dialogbit.cpp \
    view/dialog/dialogconnections.cpp \
    view/dialog/dialoggz.cpp \
    view/panel/framegz.cpp \
    view/panel/framestatus.cpp \
        main.cpp \
        view/mainwindow.cpp \
    view/panel/framecontrol1.cpp \
    view/panel/framecontrol2.cpp \
    view/panel/frameosd.cpp \
    usecase/ppi/ppiarpaobject.cpp \
    usecase/ppi/ppigzobject.cpp \
    usecase/ppi/ppiobject.cpp \
    usecase/ppi/ppicompassobject.cpp \
    usecase/stream/arpasender.cpp \
    usecase/stream/navsensor.cpp \
    usecase/stream/stream.cpp \
    usecase/stream/mqttdevicewrapper.cpp \
    usecase/stream/devicewrapper.cpp \
    view/panel/frametrackdisplay.cpp \
    view/ppi/radarwidget.cpp \
    usecase/ppi/ppievent.cpp \
    view/panel/framecontrol3.cpp \
    view/panel/framecursor.cpp \
    view/panel/frametrail.cpp \
    shared/utils.cpp

HEADERS += \
    usecase/alarm/alarm.h \
    usecase/alarm/alarmmanager.h \
    usecase/alarm/gzalarm.h \
    view/dialog/dialogbit.h \
    view/dialog/dialogconnections.h \
    view/dialog/dialoggz.h \
    view/panel/framegz.h \
    view/panel/framestatus.h \
        view/mainwindow.h \
    view/panel/framecontrol1.h \
    view/panel/framecontrol2.h \
    view/panel/frameosd.h \
    usecase/ppi/ppiarpaobject.h \
    usecase/ppi/ppigzobject.h \
    usecase/ppi/ppiobject.h \
    usecase/ppi/ppicompassobject.h \
    usecase/stream/arpasender.h \
    usecase/stream/navsensor.h \
    usecase/stream/stream.h \
    usecase/stream/mqttdevicewrapper.h \
    usecase/stream/devicewrapper.h \
    view/panel/frametrackdisplay.h \
    view/ppi/radarwidget.h \
    usecase/ppi/ppievent.h \
    view/panel/framecontrol3.h \
    view/panel/framecursor.h \
    view/panel/frametrail.h \
    shared/utils.h

FORMS += \
    view/dialog/dialogbit.ui \
    view/dialog/dialogconnections.ui \
    view/dialog/dialoggz.ui \
    view/panel/framegz.ui \
    view/panel/framestatus.ui \
        view/mainwindow.ui \
    view/panel/framecontrol1.ui \
    view/panel/framecontrol2.ui \
    view/panel/frameosd.ui \
    view/panel/frametrackdisplay.ui \
    view/panel/framecontrol3.ui \
    view/panel/framecursor.ui \
    view/panel/frametrail.ui

unix: {
    LIBS += -L/usr/lib/RadarEngine -lRadarEngine

    INCLUDEPATH += /usr/include/RadarEngine
    DEPENDPATH += /usr/include/RadarEngine

} else:win32 {
    LIBS += -lOpenGL32

    INCLUDEPATH += C:\Users\miftah\RadarEngineLib\include
    DEPENDPATH += C:\Users\miftah\RadarEngineLib\lib\include
}

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
