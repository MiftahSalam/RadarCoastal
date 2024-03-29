#ifndef GLWIDGET_H
#define GLWIDGET_H

//#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QTimer>

#include <RadarEngine/radarconfig.h>
#include <RadarEngine/global.h>
#include <RadarEngine/radarengine.h>

#include "infra/filterevent.h"
#include "ppiarpaobject.h"
#include "ppigzobject.h"
#include "ppicompassobject.h"

class RadarWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    RadarWidget(QWidget *parent = nullptr);
    ~RadarWidget() override;

    void setRectRegoin(QRect rect);

signals:
    void signal_cursorMove(const QPoint pos, const int width, const int height);
    void signal_cursorLeftRelease(const QPoint pos, const int width, const int height);

protected:
    void initializeGL() override;
    void paintEvent(QPaintEvent *event) override;
    void resizeGL(int width, int height) override;

public slots:
    void timeOut();
    void trigger_DrawSpoke(/*int transparency,*/ int angle, UINT8* data, size_t len);
    void trigger_cursorMove(const QPoint pos);
    void trigger_cursorLeftRelease(const QPoint pos);
    void trigger_contextMenu(const QPoint &g_pos, const QPoint &pos);

private:
    void drawRings(QPainter* painter, const int& side);
    void drawHM(QPainter* painter, const int& side, const bool& heading_up, const double& currentHeading);
    void drawGZ(QPainter* painter);
    void drawRingsVrm(QPainter* painter, const int curentVrm);
    void drawEbl(QPainter* painter, const int& side, const double& curentEbl);

    void saveGLState();
    void restoreGLState();
    void setupViewport(const int &width, const int &height, const QPoint &offset);

    QList<PPIObject*> drawObjects;
    FilterEvent *ppiEvent;
    RadarEngine::RadarEngine* m_re;
    RadarEngine::RadarConfig* m_instance_cfg;
#ifndef DISPLAY_ONLY_MODE
    PPIArpa *m_ppi_arpa;
#else

#endif
    QTimer *timer;
    QRect region;
    QPoint m_center_offset, m_center_point; //tes offcenter

    double cur_radar_angle_double;
    int cur_radar_angle;
    bool initGrab;
};


#endif
