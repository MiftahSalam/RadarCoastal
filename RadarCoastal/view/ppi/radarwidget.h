#ifndef GLWIDGET_H
#define GLWIDGET_H

//#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QTimer>
#include <QFutureWatcher>

#include <RadarEngine/radarconfig.h>
#include <RadarEngine/global.h>
#include <RadarEngine/radarengine.h>

#include "infra/filterevent.h"
#include "usecase/ppi/arpa.h"
#include "usecase/stream/echosender.h"
#include "view/ppi/ppiobject.h"

class RadarWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    RadarWidget(QWidget *parent = nullptr);
    ~RadarWidget() override;

    void setRectRegoin(QRect rect);
    //    void setRange(int range);
    //    void computetRingWidth();
    //    int getRange();
    //    double getRingWidth();

signals:
    //    void signal_target_param(bool r1,
    //                             int id,
    //                             double lat,
    //                             double lon,
    //                             double alt,
    //                             double rng,
    //                             double brn,
    //                             double spd,
    //                             double crs
    //                             );
    void signal_cursorMove(const QPoint pos, const int width, const int height);
    void signal_cursorLeftRelease(const QPoint pos, const int width, const int height);

protected:
    void initializeGL() override;
    void paintEvent(QPaintEvent *event) override;
    void resizeGL(int width, int height) override;

public slots:
    void timeOut();
    void trigger_DrawSpoke(/*int transparency,*/ int angle, UINT8* data, size_t len);
    //    void trigger_ReqDelTrack(bool r1,int id);
    void trigger_cursorMove(const QPoint pos);
    void trigger_cursorLeftRelease(const QPoint pos);
    void trigger_radarConfigChange(QString key, QVariant val);
    void trigger_captureFinish();

private:
    void drawRings(QPainter* painter, const int& side);
    void drawHM(QPainter* painter, const int& side, const bool& heading_up, const double& currentHeading);
    void drawGZ(QPainter* painter);

    void saveGLState();
    void restoreGLState();
    void setupViewport(int width, int height);

    QList<PPIObject*> drawObjects;
    FilterEvent *ppiEvent;
    RadarEngine::RadarEngine* m_re;
    RadarEngine::RadarConfig* m_instance_cfg;
    EchoSender *echoSender;
    QFutureWatcher<RadarEngine::CaptureResult> watcherCapture;
    PPIArpa *m_ppi_arpa;
    QTimer *timer;
    QRect region;

    RadarEngine::RadarState cur_state;
    double cur_radar_angle_double;
    int cur_radar_angle;
    bool initGrab;
    ////    int curRange;
    //    int cur_arpa_id_count,cur_arpa_id_count1;
    //    int cur_arpa_number;
    //    quint64 arpa_measure_time;
    //    quint64 arpa_measure_time1;
    //    bool old_motion_mode;
};


#endif
