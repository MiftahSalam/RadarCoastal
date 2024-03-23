#include <QtGui>
#include <QtOpenGL>
#include <QtGlobal>
#include <stdlib.h>
#include <math.h>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#include "shared/utils.h"
#include "radarwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

RadarWidget::RadarWidget(QWidget *parent)
    : QGLWidget(parent)
{
    setAutoFillBackground(false);
    setMinimumSize(200, 200);

    ppiEvent = new FilterEvent(this);
    installEventFilter(ppiEvent);
    setMouseTracking(true);

    timer  = new QTimer(this);

    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");
    m_re = RadarEngine::RadarEngine::GetInstance();
#ifndef DISPLAY_ONLY_MODE
    m_ppi_arpa = new PPIArpa(this, m_re, m_instance_cfg);
    PPIArpaObject* arpa = new PPIArpaObject(this, m_ppi_arpa);
    connect(this,&RadarWidget::signal_cursorLeftRelease,arpa->m_ppi_arpa,&PPIArpa::createMARPA);
#else
    PPIArpaObject* arpa = new PPIArpaObject(this);
#endif
    PPIGZObject* gz = new PPIGZObject(this,"GZ 1");
    PPIGZObject* gz1 = new PPIGZObject(this,"GZ 2");
    PPICompassObject* compass = new PPICompassObject(this);

    connect(ppiEvent,&FilterEvent::send_leftButtonReleased,this,&RadarWidget::trigger_cursorLeftRelease);
    connect(ppiEvent,&FilterEvent::move_mouse,this,&RadarWidget::trigger_cursorMove);
    connect(ppiEvent,&FilterEvent::send_rightButtonClicked,this,&RadarWidget::trigger_contextMenu);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));

    drawObjects<<arpa<<gz<<gz1<<compass;

    cur_radar_angle_double = 0.;
    cur_radar_angle = 0;
    initGrab = false;
    m_center_offset = QPoint(0, 0); //tes offcenter

    timer->start(100);
}

void RadarWidget::trigger_cursorMove(const QPoint pos)
{
    QPoint center_point = m_center_point-m_center_offset; //tes offenter

    qDebug()<<"pos"<<pos<<"center_point"<<center_point;

    emit signal_cursorMove(pos, 2*center_point.x(), 2*center_point.y());

//    emit signal_cursorMove(pos, width(), height());
}

void RadarWidget::trigger_cursorLeftRelease(const QPoint pos)
{
    QPoint center_point = m_center_point-m_center_offset; //tes offenter

    emit signal_cursorLeftRelease(pos, 2*center_point.x(), 2*center_point.y());

//    emit signal_cursorLeftRelease(pos, width(), height());
}

void RadarWidget::trigger_contextMenu(const QPoint &g_pos, const QPoint &pos)
{
    qDebug()<<Q_FUNC_INFO<<"pos"<<pos;
    QMenu menu(this);

    menu.addAction("Off Center",[this,pos](){
        RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::VOLATILE_PPI_ENABLE_OFF_CENTER, true); //tes offcenter
        this->m_center_offset = this->m_center_point-pos;
    });
    menu.addAction("Center",[this](){
        RadarEngine::RadarConfig::getInstance("")->setConfig(RadarEngine::VOLATILE_PPI_ENABLE_OFF_CENTER, false); //tes offcenter
        this->m_center_offset = QPoint(0,0);
    });

    menu.exec(g_pos);

}

void RadarWidget::timeOut()
{
    update();
}

void RadarWidget::drawRings(QPainter *painter, const int &side)
{
    Q_UNUSED(side)

    const int ring_step = side/RING_COUNT;
    const int ring_limit = side*2;
    int ring_total =  ring_step;

    while (ring_total < ring_limit) {
        int ring_size =  ring_total*2;
        painter->drawEllipse(-ring_total,-ring_total,ring_size,ring_size);
        ring_total += ring_step;
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

void RadarWidget::drawEbl(QPainter *painter, const int &side, const double& curentEbl)
{
    QPen curPen = painter->pen();
    QPen pen(QBrush(QColor(255,25,50,255)), curPen.width(), Qt::DashLine);
    QVector<qreal> dashes;
    qreal space = 10;
    dashes << 5 << space << 5 << space;
    pen.setDashPattern(dashes);

    painter->rotate(curentEbl-90);
    painter->setPen(pen);
    painter->drawLine(0,0,side,0);
    painter->rotate(90-curentEbl);
    painter->setPen(curPen);
}

void RadarWidget::drawRingsVrm(QPainter *painter, const int curentVrm)
{
    double curRange = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
    switch (Utils::unit) {
    case 1:
        curRange *= KM_TO_NM;
        break;
    default:
        break;
    }

    QPen curPen = painter->pen();
    QPen pen(QBrush(QColor(255,25,50,255)), curPen.width(), Qt::DashLine);
    QVector<qreal> dashes;
    qreal space = 10;
    dashes << 5 << space << 5 << space;
    pen.setDashPattern(dashes);

    painter->setPen(pen);
    int range_calc = static_cast<int>(Utils::DistanceFromCenterInPix(curentVrm, width(), height(), curRange));
    painter->drawEllipse(-range_calc,-range_calc,range_calc*2,range_calc*2);
    painter->setPen(curPen);
}

void RadarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    makeCurrent();

    setupViewport(width(), height(), m_center_offset); //tes offcenter
//    setupViewport(width(), height());

    const int preset_color = m_instance_cfg->getConfig(RadarEngine::VOLATILE_DISPLAY_PRESET_COLOR).toInt();
    /*
    if(preset_color == 0)
        glClearColor(0.f, 0.0f, 0.0f, .1f);
    else if(preset_color == 1)
        glClearColor(1.f, 1.0f, 1.0f, .1f);
    */

    QColor trans(Qt::transparent);
    glClearColor(trans.redF(), trans.greenF(), trans.blueF(), trans.alphaF());

//    setupViewport(width(), height());

    glClear(GL_COLOR_BUFFER_BIT);

    m_re->radarDraw->DrawRadarImage();

    const bool show_sweep = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_SWEEP).toBool();
    const RadarEngine::RadarState cur_state = static_cast<const RadarEngine::RadarState>(m_instance_cfg->getConfig(RadarEngine::VOLATILE_RADAR_STATUS).toInt());
    if(show_sweep && cur_state == RadarEngine::RADAR_TRANSMIT) m_re->radarDraw->DrawRadarSweep(cur_radar_angle_double);

    glShadeModel(GL_FLAT);
    glPopMatrix();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::SolidLine);
    if(preset_color == 0)
        painter.setPen(QColor(0,255,0,255));
    else if(preset_color == 1)
        painter.setPen(QColor(255,255,0,255));

    //    painter.translate(width()/2,height()/4); //tes offenter
//    painter.translate(m_center_offset); //tes offenter
//    painter.translate(width()/4,height()/4); //tes offenter
//    if(offset_enable) //off-center enabled
//        painter.translate(m_center_offset); //tes offenter
//    else
//        painter.translate(width()/2,height()/2);
    QPoint center_point = m_center_point-m_center_offset; //tes offenter
    painter.translate(center_point); //tes offenter

    int side = region.width()/2;
    const bool show_heading_marker = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_HEADING_MARKER).toBool();
    const bool show_rings = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_RING).toBool();
    const bool heading_up = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool();
    const double currentHeading = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();
    const bool show_ebl_marker = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_EBL_MARKER).toBool();
    const double curentEbl = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_EBL_VALUE).toDouble();
    const bool show_vrm_marker = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_VRM_MARKER).toBool();
    const double curentVrm = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_VRM_VALUE).toDouble();

    foreach (PPIObject* obj, drawObjects)
    {
        obj->Draw(&painter,side,width(),height(), center_point);
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

    /*
      EBL marker
    */
    if(show_ebl_marker) drawEbl(&painter, side, curentEbl);

    /*
      VRM marker
    */
    if(show_vrm_marker) drawRingsVrm(&painter, curentVrm);


}

void RadarWidget::trigger_DrawSpoke(/*int transparency,*/ int angle, UINT8 *data, size_t len)
{
//    qDebug()<<Q_FUNC_INFO<<angle;
    cur_radar_angle_double = SCALE_RAW_TO_DEGREES2048(angle);
    cur_radar_angle = angle;

    m_re->radarDraw->ProcessRadarSpoke(angle,data,len);
    update();
}

void RadarWidget::resizeGL(int width, int height)
{
    m_center_point = QPoint(width/2, height/2);
//    m_center_offset = QPoint(0, 0); //tes offcenter
//    m_center_offset = QPoint(width/3, -height/3); //tes offcenter
//    bool offset_enable = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_PPI_ENABLE_OFF_CENTER).toBool(); //tes offcenter
    setupViewport(width, height, m_center_offset);
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
    m_re->radarDraw->Init(this);
}

void RadarWidget::setupViewport(const int &width, const int &height, const QPoint &offset)
{
    int side = qMin(width, height); //scale 1
    //    int side = qMin(width, height)*2; //scale 2
    //    int side = qMin(width, height)/2; //scale 0.5
    //    glViewport(0, side / 4, side, side); //tes offcenter
        bool offset_enabled = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::VOLATILE_PPI_ENABLE_OFF_CENTER).toBool(); //tes offcenter

        if(offset_enabled)
            glViewport(-offset.x(), offset.y(), side, side); //tes offcenter
        else
            glViewport((width - side) / 2, (height - side) / 2, side, side);

    //    glViewport(-side / 4, side / 4, side, side); //tes offcenter
    //    glViewport((width - side) / 2, (height - side) / 2, side, side);
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
