#include <QtGui>
#include <QtOpenGL>
#include <QtGlobal>
#include <stdlib.h>
#include <math.h>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#include "radarwidget.h"

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
    PPIGZObject* gz = new PPIGZObject(this);
    PPICompassObject* compass = new PPICompassObject(this);

    connect(ppiEvent,&PPIEvent::send_leftButtonReleased,this,&RadarWidget::trigger_cursorLeftRelease);
    connect(ppiEvent,&PPIEvent::move_mouse,this,&RadarWidget::trigger_cursorMove);
    connect(this,&RadarWidget::signal_cursorLeftRelease,arpa,&PPIArpaObject::createMARPA);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));

    drawObjects<<arpa<<gz<<compass;

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
/*
void RadarWidget::trigger_ReqDelTrack(bool r1,int id)
{
    if(id>-10)
    {
        RA *cur_arpa = r1 ? arpa : arpa1;
        for(int i=0;i<cur_arpa->m_number_of_targets;i++)
            if(cur_arpa->m_target[i]->m_target_id == id)
                cur_arpa->m_target[i]->SetStatusLost();
    }
    else
    {
        arpa->DeleteAllTargets();
        arpa1->DeleteAllTargets();
    }
}
*/

void RadarWidget::timeOut()
{
//    Position own_pos;
//    own_pos.lat = currentOwnShipLat;
//    own_pos.lon = currentOwnShipLon;
//    Polar pol;
//    double brn;
//    double range;

//    qDebug()<<Q_FUNC_INFO;
//    if(arpa->m_number_of_targets > 0)
//    {
//        int num_limit = 5;
//        while ((cur_arpa_id_count < arpa->m_number_of_targets) && num_limit > 0)
//        {
//            if(arpa->m_target[cur_arpa_id_count]->m_target_id > 0)
//            {
//                pol = Pos2Polar(arpa->m_target[cur_arpa_id_count]->m_position,own_pos,curRange);
//                brn = SCALE_RAW_TO_DEGREES2048(pol.angle);
////                brn -= 270;
//                brn = radar_settings.headingUp ? brn+currentHeading : brn;
//                while(brn>360 || brn<0)
//                {
//                    if(brn>360)
//                        brn -= 360;
//                    if(brn<0)
//                        brn += 360;
//                }

//                double arpa_course = arpa->m_target[cur_arpa_id_count]->m_course;
////                arpa_course -= 270;
//                arpa_course = radar_settings.headingUp ? arpa_course+currentHeading : arpa_course;
//                while(arpa_course>360 || arpa_course<0)
//                {
//                    if(arpa_course>360)
//                        arpa_course -= 360;
//                    if(arpa_course<0)
//                        arpa_course += 360;
//                }

//                range = static_cast<double>(curRange*pol.r/RETURNS_PER_LINE)/1000.;
//                /* untuk menghitung posisi yang sudah dikoreksi
//                pol.angle = SCALE_DEGREES_TO_RAW2048(brn);
//                Position arpa_pos = Polar2Pos(pol,own_pos,curRange);
//                */
////                qDebug()<<Q_FUNC_INFO<<arpa->m_target[cur_arpa_id_count]->m_position.lat<<arpa->m_target[cur_arpa_id_count]->m_position.lon;
//                emit signal_target_param(true,
//                                         arpa->m_target[cur_arpa_id_count]->m_target_id,
//                                         arpa->m_target[cur_arpa_id_count]->m_position.lat,
//                                         arpa->m_target[cur_arpa_id_count]->m_position.lon,
//                                         arpa->m_target[cur_arpa_id_count]->m_position.alt, //temp
//                                         range,
//                                         brn,
//                                         arpa->m_target[cur_arpa_id_count]->m_speed_kn,
//                                         arpa_course
//                                         );
//            }
//            cur_arpa_id_count++;
//            num_limit--;
//        }
//        if(cur_arpa_id_count >= arpa->m_number_of_targets)
//            cur_arpa_id_count = 0;
//    }

//    if(old_motion_mode^radar_settings.headingUp)
//    {
//        arpa->DeleteAllTargets();
//        arpa1->DeleteAllTargets();
//        old_motion_mode = radar_settings.headingUp;
//    }

    update();
}

void RadarWidget::drawRings(QPainter *painter, const int &side)
{
    int ringCount = qCeil(side/2)-20;
    int bufRng = ringCount;

//    painter->setPen(QColor(255,255,0,100));
    for(int i=0;i<RING_COUNT;i++)
    {
        painter->drawEllipse(-bufRng/2,-bufRng/2,bufRng,bufRng);
        bufRng += ringCount;
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

//    saveGLState();

    m_re->radarDraw->DrawRadarImage();

    const bool show_sweep = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::NON_VOLATILE_PPI_DISPLAY_SHOW_SWEEP).toBool();
    if(show_sweep) m_re->radarDraw->DrawRadarSweep(cur_radar_angle);

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


//    restoreGLState();
//    painter.rotate(-bearing+180);

//    painter.end();
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

    /*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES
    glOrthof(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
    */
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
