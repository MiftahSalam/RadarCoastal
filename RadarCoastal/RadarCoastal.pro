#-------------------------------------------------
#
# Project created by QtCreator 2022-03-31T15:41:56
#
#-------------------------------------------------

QT       += core gui opengl network websockets qmqtt

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
#CONFIG += QMQTT_WEBSOCKETS
DEFINES += SAVE_CAPTURE
DEFINES += USE_LOG4QT

#MODE += DEBUG_MODE

equals(MODE,"DEBUG_MODE") {
    TARGET = RadarCoastal-console
    CONFIG += console
}
else {
    TARGET = RadarCoastal
}

#include(infra/qmqtt/qmqtt.pri)

SOURCES += \
#    infra/withconfig.cpp \
#    infra/withradarengine.cpp \
    domain/track/trackmodel.cpp \
    infra/database/track/memory/trackrepository_mem.cpp \
    infra/stream/wsdevicewrapper.cpp \
    usecase/alarm/alarm.cpp \
    usecase/alarm/alarmmanager.cpp \
    usecase/alarm/gzalarm.cpp \
    usecase/ppi/arpa.cpp \
    usecase/stream/arpa_sender_model.cpp \
    usecase/stream/nav_data_model.cpp \
    usecase/stream/sitedatasender.cpp \
    usecase/track/track.cpp \
    usecase/track/trackmodelview.cpp \
    usecase/tx_timer/tx_timer_counter.cpp \
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
    usecase/stream/echosender.cpp \
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
    infra/stream/wsdevicewrapper.h \
    usecase/alarm/alarm.h \
    usecase/alarm/alarmmanager.h \
    usecase/alarm/gzalarm.h \
    usecase/ppi/arpa.h \
    usecase/stream/arpa_sender_model.h \
    usecase/stream/baseresponsejson.h \
    usecase/stream/nav_data_model.h \
    usecase/stream/sitedatasender.h \
    usecase/track/track.h \
    usecase/track/trackmodelview.h \
    usecase/tx_timer/tx_timer_counter.h \
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
    usecase/stream/echosender.h \
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

TRANSLATIONS += \
    translantions/i18n_en.ts \
    translantions/i18n_id.ts

unix: {
    LIBS += -L/usr/lib/hypernet-2024/RadarEngine -lRadarEngine

    INCLUDEPATH += /usr/include/hypernet-2024
    DEPENDPATH += /usr/include/hypernet-2024

    contains(DEFINES, USE_LOG4QT) {
        message(Using log4qt...)
        LIBS += -L/usr/local/log4qt/lib/ -llog4qt
        INCLUDEPATH += /usr/local/log4qt/include
        DEPENDPATH += /usr/local/log4qt/include
    }
} else:win32 {
    LIBS += -lOpenGL32

    INCLUDEPATH +=C:\Users\miftah\RadarEngineLib\include\2024\hypernet\v2
    DEPENDPATH += C:\Users\miftah\RadarEngineLib\lib\include\2024\hypernet\v2
#    INCLUDEPATH +=C:\Users\ms_tensai\RadarEngineLib\include\2024\hypernet\v2
#    DEPENDPATH += C:\Users\ms_tensai\RadarEngineLib\lib\include\2024\hypernet\v2

    contains(DEFINES, USE_LOG4QT) {
        message(Using log4qt...)
        LIBS += -LC:\log4qt\lib -llog4qt
        INCLUDEPATH += C:\log4qt\include
        DEPENDPATH += C:\log4qt\include
    }

}

win32:CONFIG(release, debug|release): LIBS += -LC:\Users\miftah\RadarEngineLib\lib\2024\hypernet\v2 -lRadarEngine
else:win32:CONFIG(debug, debug|release): LIBS += -LC:\Users\miftah\RadarEngineLib\lib\2024\hypernet\v2 -lRadarEngine
#win32:CONFIG(release, debug|release): LIBS += -LC:\Users\ms_tensai\RadarEngineLib\lib\2024\hypernet\v2 -lRadarEngine
#else:win32:CONFIG(debug, debug|release): LIBS += -LC:\Users\ms_tensai\RadarEngineLib\lib\2024\hypernet\v2 -lRadarEngine


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#DISTFILES += \
#    HMI_style.css

RESOURCES += \
    assets.qrc \
    i18n.qrc

DISTFILES += \
    translantions/i18n_en.ts \
    translantions/i18n_id.ts
