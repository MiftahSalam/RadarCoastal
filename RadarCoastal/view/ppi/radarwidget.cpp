#include <QtGui>
#include <QtOpenGL>
#include <QtGlobal>
#include <QtConcurrent/QtConcurrent>
#include <stdlib.h>
#include <math.h>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#include "shared/utils.h"
#include "view/ppi/ppiarpaobject.h"
#include "view/ppi/ppigzobject.h"
#include "view/ppi/ppicompassobject.h"
#include "radarwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

/*
const char fShader [] =
        "uniform sampler2D texture1;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
        "gl_FragColor = texture2D(texture1, texc.st);\n"
        "};\n"
        ;

const char vShader [] =
        "attribute highp vec4 aPos;\n"
        "attribute mediump vec4 aTexCoord;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
            "gl_Position = aPos;\n"
            "texc = aTexCoord;\n"
        "}\n"
        ;
*/
RadarWidget::RadarWidget(QWidget *parent)
    : QGLWidget(parent)
{
    setAutoFillBackground(false);
    setMinimumSize(200, 200);

    auto format = QGLFormat::defaultFormat();
    format.setAlpha(true);
    setFormat(format);

    ppiEvent = new FilterEvent(this);
    installEventFilter(ppiEvent);
    setMouseTracking(true);

    timer = new QTimer(this);

    m_instance_cfg = RadarEngine::RadarConfig::getInstance("");
    m_re = RadarEngine::RadarEngine::GetInstance();
    m_ppi_arpa = new PPIArpa(this, m_re, m_instance_cfg);
    echoSender = new EchoSender(this);

    PPIArpaObject *arpa = new PPIArpaObject(this, m_ppi_arpa);
    map = new Map(this);

    PPIGZObject *gz = new PPIGZObject(this, "GZ 1");
    PPIGZObject *gz1 = new PPIGZObject(this, "GZ 2");
    PPICompassObject *compass = new PPICompassObject(this);

    connect(&watcherCapture, &QFutureWatcher<RadarEngine::CaptureResult>::finished, this, &RadarWidget::trigger_captureFinish);
    connect(ppiEvent, &FilterEvent::send_leftButtonReleased, this, &RadarWidget::trigger_cursorLeftRelease);
    connect(ppiEvent, &FilterEvent::move_mouse, this, &RadarWidget::trigger_cursorMove);
    connect(this, &RadarWidget::signal_cursorLeftRelease, arpa->m_ppi_arpa, &PPIArpa::createMARPA);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));
    connect(m_instance_cfg, &RadarEngine::RadarConfig::configValueChange, this, &RadarWidget::trigger_radarConfigChange);

    drawObjects << arpa << gz << gz1 << compass;

    cur_state = RadarEngine::RADAR_OFF;
    cur_radar_angle_double = 0.;
    cur_radar_angle = 0;
    initGrab = false;

    timer->start(100);
}

void RadarWidget::trigger_radarConfigChange(QString key, QVariant val)
{
    if (key == RadarEngine::VOLATILE_RADAR_STATUS)
    {
        RadarEngine::RadarState state = static_cast<RadarEngine::RadarState>(val.toInt());
        if (state != cur_state && state == RadarEngine::RADAR_TRANSMIT)
        {
            initGrab = true;
        }
        else if (state != cur_state && state != RadarEngine::RADAR_TRANSMIT)
        {
            echoSender->m_ppi_grabber->stop();
            //            echoSender->m_re->m_radar_capture->stop();
        }
        cur_state = state;
    }
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
    echoSender->Reconnect();

    update();
}

void RadarWidget::drawRings(QPainter *painter, const int &side)
{
    Q_UNUSED(side)
    double range = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
    const quint8 unit = static_cast<quint8>(m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_UNIT).toUInt());

    switch (unit)
    {
    case 1:
        range *= KM_TO_NM;
        break;
    default:
        break;
    }

    const qreal range_ring = range / RING_COUNT;

    //    painter->setPen(QColor(255,255,0,100));
    for (int i = 0; i < RING_COUNT; i++)
    {
        int range_calc = static_cast<int>(Utils::DistanceFromCenterInPix(range_ring * (i + 1), width(), height(), range) / 1.);
        //        qDebug()<<Q_FUNC_INFO<<"bufRng"<<bufRng;
        //        qDebug()<<Q_FUNC_INFO<<"bufRng calc"<<range_calc;
        painter->drawEllipse(-range_calc, -range_calc, range_calc * 2, range_calc * 2);
        //        painter->drawEllipse(-bufRng/2,-bufRng/2,bufRng,bufRng);
        //        bufRng += ringCount;
    }
}

void RadarWidget::drawHM(QPainter *painter, const int &side, const bool &heading_up, const double &currentHeading)
{
    double baringan = heading_up ? 0 : currentHeading;
    painter->rotate(baringan - 90);
    //        painter.rotate(baringan-180);
    //    painter->setPen(QColor(255,255,0,255));
    painter->drawLine(0, 0, side, 0);
    //        painter.rotate(180-baringan);
    painter->rotate(90 - baringan);
}
/*
void RadarWidget::drawTexture()
{
    m_text->load(map->m_current_grab);

    vbo.create();
    vbo.bind();
    vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));

    m_environmentProgram->enableAttributeArray(0);
    m_environmentProgram->enableAttributeArray(1);
    m_environmentProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_environmentProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    m_environmentProgram->bind();
    m_text->bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    m_text->unbind();
    vbo.release();
    m_environmentProgram->disableAttributeArray(0);
    m_environmentProgram->disableAttributeArray(1);
    m_environmentProgram->release();
}
*/
void RadarWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    makeCurrent();

    const int preset_color = m_instance_cfg->getConfig(RadarEngine::VOLATILE_DISPLAY_PRESET_COLOR).toInt();
    /*
    if(preset_color == 0)
        glClearColor(0.f, 0.0f, 0.0f, .1f);
    else if(preset_color == 1)
        glClearColor(1.f, 1.0f, 1.0f, .1f);
    */

    QColor trans(Qt::transparent);
    glClearColor(trans.redF(), trans.greenF(), trans.blueF(), trans.alphaF());

    setupViewport(width(), height());

    glClear(GL_COLOR_BUFFER_BIT);

    map->drawTexture();

    m_re->radarDraw->DrawRadarImage();

    if (echoSender->m_re->m_radar_capture->isStart() && echoSender->m_re->m_radar_capture->pendingGrabAvailable())
    {
        qDebug() << Q_FUNC_INFO << "gl format rgba" << format().rgba();
        qDebug() << Q_FUNC_INFO << "gl format alpha" << format().alpha();

        auto echo = grabFrameBuffer(true);
        //        auto echo = echoSender->m_re->m_radar_capture->readPixel(width(), height());

        //        QFuture<RadarEngine::CaptureResult> future = QtConcurrent::run(m_re->m_radar_capture, &RadarEngine::RadarImageCapture::capture, echo, width(), height());
        QFuture<RadarEngine::CaptureResult> future = QtConcurrent::run(m_re->m_radar_capture, &RadarEngine::RadarImageCapture::capture, echo);
        watcherCapture.setFuture(future);
    }
    /*
    if(echoSender->m_ppi_grabber->isStart() && echoSender->m_ppi_grabber->pendingGrabAvailable())
    {
        echoSender->m_ppi_grabber->grab(grabFrameBuffer(true));
    }
    */
    const bool show_sweep = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_SWEEP).toBool();
    const RadarEngine::RadarState cur_state = static_cast<const RadarEngine::RadarState>(m_instance_cfg->getConfig(RadarEngine::VOLATILE_RADAR_STATUS).toInt());
    if (show_sweep && cur_state == RadarEngine::RADAR_TRANSMIT)
        m_re->radarDraw->DrawRadarSweep(cur_radar_angle_double);

    glShadeModel(GL_FLAT);
    glPopMatrix();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::SolidLine);
    if (preset_color == 0)
        painter.setPen(QColor(0, 255, 0, 255));
    else if (preset_color == 1)
        painter.setPen(QColor(255, 255, 0, 255));
    painter.translate(width() / 2, height() / 2);

    int side = region.width() / 2;
    const bool show_heading_marker = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_HEADING_MARKER).toBool();
    const bool show_rings = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_RING).toBool();
    const bool heading_up = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool();
    const double currentHeading = m_instance_cfg->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();

    foreach (PPIObject *obj, drawObjects)
    {
        obj->Draw(&painter, side);
    }

    /*ring boundary*/
    int ring_size = qCeil(2 * side) - 5;
    painter.drawEllipse(-ring_size / 2, -ring_size / 2, ring_size, ring_size);

    /*
      heading marker
    */
    if (show_heading_marker)
        drawHM(&painter, side, heading_up, currentHeading);

    /*
      range ring
    */
    if (show_rings)
        drawRings(&painter, side);
}

void RadarWidget::trigger_captureFinish()
{
    QFutureWatcher<RadarEngine::CaptureResult> *watcher;
    watcher = reinterpret_cast<QFutureWatcher<RadarEngine::CaptureResult> *>(sender());
    RadarEngine::CaptureResult result = watcher->result();
    echoSender->sendDataAsync(result);
}

void RadarWidget::trigger_DrawSpoke(/*int transparency,*/ int angle, UINT8 *data, size_t len)
{
    //    qDebug()<<Q_FUNC_INFO<<angle;
    cur_radar_angle_double = SCALE_RAW_TO_DEGREES2048(angle);
    cur_radar_angle = angle;

    if (initGrab)
    {
        //        echoSender->m_re->m_radar_capture->start();
        echoSender->m_ppi_grabber->start();
        initGrab = false;
    }
    //    if(echoSender->m_re->m_radar_capture->isStart()) echoSender->m_re->m_radar_capture->update();
    if (echoSender->m_ppi_grabber->isStart())
        echoSender->m_ppi_grabber->update();

    m_re->radarDraw->ProcessRadarSpoke(angle, data, len);
    update();
}

void RadarWidget::resizeGL(int width, int height)
{
    setupViewport(width, height);
    map->resize(QSize(width, height));
}
void RadarWidget::setRectRegoin(QRect rect)
{
    region = QRect(0, 0, rect.width(), rect.width());
}

RadarWidget::~RadarWidget()
{
}

void RadarWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.f, 0.0f, 0.0f, .1f);

    //    m_vertexShader = new QGLShader(QGLShader::Vertex);
    //    m_vertexShader->compileSourceCode(vShader);

    //    m_fragmentShaders = new QGLShader(QGLShader::Fragment);
    //    m_fragmentShaders->compileSourceCode(fShader);

    //    m_environmentProgram = new QGLShaderProgram;
    //    m_environmentProgram->addShader(m_vertexShader);
    //    m_environmentProgram->addShader(m_fragmentShaders);
    //    m_environmentProgram->bindAttributeLocation("aPos", 0);
    //    m_environmentProgram->bindAttributeLocation("aTexCoord", 1);
    //    m_environmentProgram->link();
    //    m_environmentProgram->bind();
    //    m_environmentProgram->setUniformValue("texture1", 0);

    /*
    float vertices[4][8] = {
        {-1.f,  1.f, 0.0f,   0.25f, 0.25f},
        {-1.f, -1.f, 0.0f,   0.25f, .75f},
        {1.f, -1.f, 0.0f,  0.75f, 0.75f},
        {1.f,  1.f, 0.0f,  .75f, 0.25f}
    };
    */
    /*

    float vertices[4][8] = {
        {-1.f, 1.f, 0.0f, 0.f, 0.f},
        {-1.f, -1.f, 0.0f, 0.f, 1.0f},
        {1.f, -1.f, 0.0f, 1.0f, 1.0f},
        {1.f, 1.f, 0.0f, 1.0f, 0.f}};
    for (int j = 0; j < 4; ++j)
    {
        vertData.append(vertices[j][0]);
        vertData.append(vertices[j][1]);
        vertData.append(vertices[j][2]);

        vertData.append(vertices[j][3]);
        vertData.append(vertices[j][4]);
    }

    m_text = new GLTextureCube(width(),height());
    */
    map->initGl();

    glClearColor(0.f, 0.0f, 0.0f, 1.0f);
#ifdef QT_OPENGL_ES
    glOrthof(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
#endif

#ifdef QT_OPENGL_ES
    glOrthof(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, 0.5, 4.0, 15.0);
#endif
    m_re->radarDraw->Init(this);
}

void RadarWidget::setupViewport(int width, int height)
{
    int side = qMin(width, height); // scale 1
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
