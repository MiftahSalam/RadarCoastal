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
#    infra/withconfig.cpp \
#    infra/withradarengine.cpp \
    domain/track/trackmodel.cpp \
    infra/database/track/memory/trackrepository_mem.cpp \
    usecase/alarm/alarm.cpp \
    usecase/alarm/alarmmanager.cpp \
    usecase/alarm/gzalarm.cpp \
    usecase/ppi/arpa.cpp \
    usecase/ppi/ppigrabber.cpp \
    usecase/stream/echosender.cpp \
    usecase/track/track.cpp \
    usecase/track/trackmodelview.cpp \
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
    usecase/stream/arpasender.cpp \
    usecase/stream/navsensor.cpp \
    infra/stream/stream.cpp \
    infra/stream/mqttdevicewrapper.cpp \
    infra/stream/devicewrapper.cpp \
    infra/filterevent.cpp \
    view/panel/frametrackdisplay.cpp \
    view/ppi/ppiarpaobject.cpp \
    view/ppi/ppicompassobject.cpp \
    view/ppi/ppigzobject.cpp \
    view/ppi/ppiobject.cpp \
    view/ppi/radarwidget.cpp \
    view/panel/framecontrol3.cpp \
    view/panel/framecursor.cpp \
    view/panel/frametrail.cpp \
    shared/utils.cpp

HEADERS += \
#    infra/withconfig.h \
#    infra/withradarengine.h \
    domain/track/trackmodel.h \
    domain/track/trackrepository.h \
    infra/database/track/memory/trackrepository_mem.h \
    usecase/alarm/alarm.h \
    usecase/alarm/alarmmanager.h \
    usecase/alarm/gzalarm.h \
    usecase/ppi/arpa.h \
    usecase/ppi/ppigrabber.h \
    usecase/stream/echosender.h \
    usecase/track/track.h \
    usecase/track/trackmodelview.h \
    view/dialog/dialogbit.h \
    view/dialog/dialogconnections.h \
    view/dialog/dialoggz.h \
    view/panel/framegz.h \
    view/panel/framestatus.h \
    view/mainwindow.h \
    view/panel/framecontrol1.h \
    view/panel/framecontrol2.h \
    view/panel/frameosd.h \
    usecase/stream/arpasender.h \
    usecase/stream/navsensor.h \
    infra/stream/stream.h \
    infra/stream/mqttdevicewrapper.h \
    infra/stream/devicewrapper.h \
    infra/filterevent.h \
    view/panel/frametrackdisplay.h \
    view/ppi/ppiarpaobject.h \
    view/ppi/ppicompassobject.h \
    view/ppi/ppigzobject.h \
    view/ppi/ppiobject.h \
    view/ppi/radarwidget.h \
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

    INCLUDEPATH +=C:\Users\ms_tensai\RadarEngineLib\include\2023\v2
    DEPENDPATH += C:\Users\ms_tensai\RadarEngineLib\lib\include\2023\v2
}

win32:CONFIG(release, debug|release): LIBS += -LC:\Users\ms_tensai\RadarEngineLib\lib\2023\v2 -lRadarEngine
else:win32:CONFIG(debug, debug|release): LIBS += -LC:\Users\ms_tensai\RadarEngineLib\lib\2023\v2 -lRadarEngine


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#DISTFILES += \
#    HMI_style.css

RESOURCES += \
    assets.qrc
