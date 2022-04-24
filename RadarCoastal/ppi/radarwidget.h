#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QTimer>

#include <radarconfig.h>
#include <radarengine_global.h>
#include <radarengine.h>

#include "ppi/ppievent.h"
#include "ppiarpaobject.h"

class RadarWidget : public QOpenGLWidget, protected QOpenGLFunctions
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

protected:
    void initializeGL();
    void paintEvent(QPaintEvent *event);
    void resizeGL(int width, int height);
//    void paintGL();
//    void mouseReleaseEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);

public slots:
    void timeOut();
    void trigger_DrawSpoke(/*int transparency,*/ int angle, UINT8* data, size_t len);
//    void trigger_ReqDelTrack(bool r1,int id);
    void trigger_cursorMove(const QPointF pos);

private:
    void drawCompass(QPainter* painter, const int& side, const bool& heading_up, const double& currentHeading);
    void drawRings(QPainter* painter, const int& side);
    void drawHM(QPainter* painter, const int& side, const bool& heading_up, const double& currentHeading);
    void drawArpa(QPainter* painter);
    void drawGZ(QPainter* painter);

    void saveGLState();
    void restoreGLState();
    void setupViewport(int width, int height);
//    void createMARPA(QPoint pos);

    QList<PPIObject*> drawObjects;
    PPIEvent *ppiEvent;
    RadarEngine::RadarEngine* m_re;
    QTimer *timer;
    QRect region;

//    double ringWidth;
    double cur_radar_angle;
////    int curRange;
//    int cur_arpa_id_count,cur_arpa_id_count1;
//    int cur_arpa_number;
//    quint64 arpa_measure_time;
//    quint64 arpa_measure_time1;
//    qint64 dummy_timeout;
//    bool old_motion_mode;

//    int old_x1;
//    int old_y1;

//    int counter;
};


#endif
