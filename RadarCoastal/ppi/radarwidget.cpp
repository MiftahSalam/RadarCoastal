#include <QtGui>
#include <QtOpenGL>
#include <QtGlobal>
#include <stdlib.h>
#include <math.h>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#include "radarwidget.h"
#include "utils.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

RadarWidget::RadarWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setAutoFillBackground(false);
    setMinimumSize(200, 200);

    QSurfaceFormat format;
    format.setSamples(16);
    setFormat(format);

    ppiEvent = new PPIEvent(this);
    installEventFilter(ppiEvent);
    setMouseTracking(true);

    timer  = new QTimer(this);

    m_re = RadarEngine::RadarEngine::getInstance();
    PPIArpaObject* arpa = new PPIArpaObject(this);
    PPIGZObject* gz = new PPIGZObject(this,"GZ 1");
    PPIGZObject* gz1 = new PPIGZObject(this,"GZ 2");
    PPICompassObject* compass = new PPICompassObject(this);

    connect(ppiEvent,&PPIEvent::send_leftButtonReleased,this,&RadarWidget::trigger_cursorLeftRelease);
    connect(ppiEvent,&PPIEvent::move_mouse,this,&RadarWidget::trigger_cursorMove);
    connect(this,&RadarWidget::signal_cursorLeftRelease,arpa,&PPIArpaObject::createMARPA);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));

    drawObjects<<arpa<<gz<<gz1<<compass;

    cur_radar_angle = 0.;
    timer->start(100);

//    old_motion_mode = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool();
}

void RadarWidget::trigger_cursorMove(const QPoint pos)
{
    emit signal_cursorMove(pos, width(), height());
}
void RadarWidget::trigger_cursorLeftRelease(const QPoint pos)
{
    emit signal_cursorLeftRelease(pos, width(), height());
}

void RadarWidget::timeOut()
{
    update();
}

void RadarWidget::drawRings(QPainter *painter, const int &side)
{
    Q_UNUSED(side)
//    int ringCount = qCeil(side/2)-20;
//    int bufRng = ringCount;
    const qreal range = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble()/1000.;
    const qreal range_ring = range/RING_COUNT;

//    painter->setPen(QColor(255,255,0,100));
    for(int i=0;i<RING_COUNT;i++)
    {
        int range_calc = static_cast<int>(distanceFromCenterInPix(range_ring*(i+1),width(), height(), range)/1.);
//        qDebug()<<Q_FUNC_INFO<<"bufRng"<<bufRng;
//        qDebug()<<Q_FUNC_INFO<<"bufRng calc"<<range_calc;
        painter->drawEllipse(-range_calc,-range_calc,range_calc*2,range_calc*2);
//        painter->drawEllipse(-bufRng/2,-bufRng/2,bufRng,bufRng);
//        bufRng += ringCount;
    }

}

void RadarWidget::drawHM(QPainter *painter, const int &side, const bool& heading_up, const double& currentHeading)
{
    double baringan = heading_up ? 0 : currentHeading;
    painter->rotate(baringan-90);
    //        painter.rotate(baringan-180);
//    painter->setPen(QColor(255,255,0,255));
    painter->drawLine(0,0,side,0);
    //        painter.rotate(180-baringan);
    painter->rotate(90-baringan);
}


void RadarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    makeCurrent();

    const int preset_color = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_DISPLAY_PRESET_COLOR).toInt();

    setupViewport(width(), height());
    if(preset_color == 0)
        glClearColor(0.f, 0.0f, 0.0f, .1f);
    else if(preset_color == 1)
        glClearColor(1.f, 1.0f, 1.0f, .1f);

    glClear(GL_COLOR_BUFFER_BIT);

    m_re->radarDraw->DrawRadarImage();

    const bool show_sweep = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_SWEEP).toBool();
    const RadarEngine::RadarState cur_state = static_cast<const RadarEngine::RadarState>(RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_RADAR_STATUS).toInt());
    if(show_sweep && cur_state == RadarEngine::RADAR_TRANSMIT) m_re->radarDraw->DrawRadarSweep(cur_radar_angle);

    glShadeModel(GL_FLAT);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::SolidLine);
    if(preset_color == 0)
        painter.setPen(QColor(0,255,0,255));
    else if(preset_color == 1)
        painter.setPen(QColor(255,255,0,255));
    painter.translate(width()/2,height()/2);

    int side = region.width()/2;
    const bool show_heading_marker = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_HEADING_MARKER).toBool();
    const bool show_rings = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_RING).toBool();
    const bool heading_up = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool();
    const double currentHeading = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();

    foreach (PPIObject* obj, drawObjects)
    {
        obj->draw(&painter,side);
    }

    /*ring boundary*/
    int ring_size = qCeil(2*side)-5;
    painter.drawEllipse(-ring_size/2,-ring_size/2,ring_size,ring_size);

    /*
      heading marker
    */
    if(show_heading_marker) drawHM(&painter, side, heading_up, currentHeading);

    /*
      range ring
    */
    if(show_rings) drawRings(&painter,side);

}

void RadarWidget::trigger_DrawSpoke(/*int transparency,*/ int angle, UINT8 *data, size_t len)
{
//    qDebug()<<Q_FUNC_INFO<<angle;
    cur_radar_angle = SCALE_RAW_TO_DEGREES2048(angle);
    m_re->radarDraw->ProcessRadarSpoke(angle,data,len);
    update();
}

void RadarWidget::resizeGL(int width, int height)
{
    setupViewport(width, height);
}
void RadarWidget::setRectRegoin(QRect rect)
{
    region = QRect(0,0,rect.width(),rect.width());
}

RadarWidget::~RadarWidget()
{
}

void RadarWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.f, 0.0f, 0.0f, .1f);
    #ifdef QT_OPENGL_ES
        glOrthof(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
    #else
        glOrtho(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
    #endif
        m_re->radarDraw->init(this);
}

void RadarWidget::setupViewport(int width, int height)
{
    int side = qMin(width, height); //scale 1
//    int side = qMin(width, height)*2; //scale 2
//    int side = qMin(width, height)/2; //scale 0.5
    glViewport((width - side) / 2, (height - side) / 2, side, side);

}

void RadarWidget::saveGLState()
{
    glEnable(GL_NORMALIZE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void RadarWidget::restoreGLState()
{
    glDisable(GL_NORMALIZE);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}
