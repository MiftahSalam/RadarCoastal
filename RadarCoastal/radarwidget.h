#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QTimer>

#include <radarengine_global.h>
#include <radarengine.h>
//#include "qtmosq.h"


class RadarWidget : public QGLWidget
{
    Q_OBJECT

public:
    RadarWidget(QWidget *parent = 0, RI *ri = 0, RI *ri1=0);
    ~RadarWidget();

    void setRectRegoin(QRect rect);
    void setRange(int range);
    void computetRingWidth();
    int getRange();
    double getRingWidth();
    void trigger_shutdown();

signals:
    void signal_target_param(bool r1,
                             int id,
                             double lat,
                             double lon,
                             double alt,
                             double rng,
                             double brn,
                             double spd,
                             double crs
                             );
    void signal_adsb_param(QString icao,double spd, double crs, double lat, double lon,double alt);
    void signal_cursorMove(double range,double brn);

protected:
    void initializeGL();
    void paintEvent(QPaintEvent *event);
    void resizeGL(int width, int height);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

public slots:
    void timeOut();
    void trigger_DrawSpoke(int transparency, int angle, UINT8* data, size_t len);
    void trigger_DrawSpoke1(int transparency, int angle, UINT8* data, size_t len);
    void trigger_ReqDelTrack(bool r1,int id);
    void trigger_simTriggered();

private:
    void setupViewport(int width, int height);
    void createMARPA(QPoint pos);

    RD *spokeDrawer;
    RD *spokeDrawer1;
    RI *m_ri;
    RI *m_ri1;
    RA *arpa;
    RA *arpa1;
    QTimer *timer;
    QRect region;
    QUdpSocket *simUdpSocket;
    QString append_data_sim;
    Position dummy_pos;

    double ringWidth;
    double cur_radar_angle,cur_radar_angle1;
    int curRange;
    int cur_arpa_id_count,cur_arpa_id_count1;
    int cur_arpa_number;
    quint64 arpa_measure_time;
    quint64 arpa_measure_time1;
    qint64 dummy_timeout;
    bool old_motion_mode;

    int old_x1;
    int old_y1;

    int counter;
};


#endif
