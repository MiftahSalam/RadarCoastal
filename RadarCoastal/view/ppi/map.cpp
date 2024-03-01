#include "map.h"

#include <QMapControl/LayerMapAdapter.h>
#include <QMapControl/MapAdapterGoogle.h>
#include <QMapControl/MapAdapterBing.h>

#include <QApplication>

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

QList<double> scalebar_distances = QList<double>()
                << 5000000.0
                << 2000000.0
                << 1000000.0
                << 1000000.0
                << 1000000.0
                << 100000.0
                << 100000.0
                << 50000.0
                << 50000.0
                << 10000.0
                << 10000.0
                << 10000.0
                << 1000.0
                << 1000.0
                << 500.0
                << 200.0
                << 100.0
                << 50.0
                << 25.0;

/*
 * radar 40.000 -> map 50000(zoom 8), scale 0.8, scale px 1.04829
 * radar 30.000 -> map 10000(zoom 9), scale 3, scale px 0.56
 * radar 20.000 -> map 10000(zoom 10), scale 3, scale px 1.67727
 * radar 10.000 -> map 10000(zoom 11), scale 1, scale px 6.88562
 * radar 5.000 -> map 1000(zoom 12), scale 5, scale px 2.90726
 * radar 2.000 -> map 1000(zoom 13), scale 2, scale px 17.4436
 * radar 1.500 -> map 500(zoom 14), scale 3, scale px 26.1653,
 * radar 1.000 -> map 200(zoom 15), scale 5, scale px 41.8645,
 * radar 500 -> map 100(zoom 16), scale 5, scale px 42.8693
*/
QMap<int, int> radarZoomMap;
QMap<int, double> radarScaleMap;

Map::Map(QObject *parent)
    : QObject{parent}
{
    /*
    radarZoomMap.insert(40000, 11); //34940 vs 35343 ~ 0.988
    radarZoomMap.insert(30000, 11); //25960 vs 35343 ~ 0.73
    radarZoomMap.insert(20000, 12); //13500 vs 13870 ~ 0.97
    radarZoomMap.insert(10000, 13); //8010 vs 8292 ~ 0.96
    radarZoomMap.insert(5000, 14); //3830m vs 3891m ~ 0.94
    radarZoomMap.insert(2000, 15); //1260m vs 1597m ~ 0.788
    radarZoomMap.insert(1500, 16); //940m vs 874m ~ 1.07
    radarZoomMap.insert(1000, 16); //680m vs 874m ~ 0.778
    radarZoomMap.insert(500, 17); //250m vs 325m ~ 0.76

    radarScaleMap.insert(40000, 34940./35343.); //34940 vs 35343 ~ 0.988
    radarScaleMap.insert(30000, 25960./35343.); //25960 vs 35343 ~ 0.73
    radarScaleMap.insert(20000, 13500./13870.); //13500 vs 13870 ~ 0.97
    radarScaleMap.insert(10000, 8010./8292.); //8010 vs 8292 ~ 0.96
    radarScaleMap.insert(5000, 3830./3891.); //3830m vs 3891m ~ 0.94
    radarScaleMap.insert(2000, 1260./1597.); //1260m vs 1597m ~ 0.788
    radarScaleMap.insert(1500, 940./874.); //940m vs 874m ~ 1.07
    radarScaleMap.insert(1000, 680./874.); //680m vs 874m ~ 0.778
    radarScaleMap.insert(500, 250./325.); //250m vs 325m ~ 0.76
    */

    radarZoomMap.insert(40000, 11); //29500 vs 30062 ~
    radarZoomMap.insert(30000, 12); //25950 vs 17784 ~
    radarZoomMap.insert(20000, 12); //17270 vs 17784 ~
    radarZoomMap.insert(10000, 13); //6910 vs 7110 ~
    radarZoomMap.insert(5000, 15); //3780 vs 1970 ~
    radarZoomMap.insert(2000, 16); //1360 vs 908 ~
    radarZoomMap.insert(1500, 16); //1020 vs 908 ~
    radarZoomMap.insert(1000, 17); //860 vs 588 ~
    radarZoomMap.insert(500, 17); //430 vs 588 ~

    radarScaleMap.insert(40000, 29500./30062.); //29500 vs 30217 ~
    radarScaleMap.insert(30000, 85950./17784.); //15620 vs 10601 ~
    radarScaleMap.insert(20000, 17270./17784.); //10450 vs 10601 ~
    radarScaleMap.insert(10000, 6910./7110.); //5340 vs 5497 ~
    radarScaleMap.insert(5000, 3780./1970.); //3150 vs 1593 ~
    radarScaleMap.insert(2000, 1360./908.); //130 vs 89 ~
    radarScaleMap.insert(1500, 1020./908.); //130 vs 89 ~
    radarScaleMap.insert(1000, 860./588.); //140 vs 89 ~
    radarScaleMap.insert(500, 430./588.); //75 vs 89 ~

    m_radar_config = RadarEngine::RadarConfig::getInstance("");
    connect(m_radar_config,&RadarEngine::RadarConfig::configValueChange,
            this,&Map::triggerConfigChange);

    mc = new QMapControl(QSizeF(400,400));
    mc->enablePersistentCache();
    mc->enableZoomControls(false);

    std::shared_ptr<LayerMapAdapter> mapLayer(std::make_shared<LayerMapAdapter>("layer view"));
//    mapLayer->setMapAdapter(std::make_shared<MapAdapterBing>());
//    mapLayer->setMapAdapter(std::make_shared<MapAdapterGoogle>(MapAdapterGoogle::GoogleLayerType::SATELLITE_ONLY));
    mapLayer->setMapAdapter(std::make_shared<MapAdapterGoogle>());

    mc->addLayer(mapLayer);

    center.setLatitude(m_radar_config->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble());
    center.setLongitude(m_radar_config->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble());
    currentRange = m_radar_config->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt();
    currentZoom = radarZoomMap.value(currentRange);
    currentScale = radarScaleMap.value(currentRange);
    loading = true;

//    calculateScale();

    mc->setMapFocusPoint(center);
    mc->setViewportSize(QSizeF(400,400));
    mc->setZoom(currentZoom);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Map::onTimeout);
    timer->start(1000);
}

void Map::triggerConfigChange(const QString key, const QVariant val)
{
    if(key == RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE)
    {
        int range = val.toInt();
        /*
        if(range < currentRange) mc->zoomIn(); //temp
        else if(range > currentRange) mc->zoomOut(); //temp
        */
        currentRange = range;
        currentZoom = radarZoomMap.value(currentRange);
        currentScale = radarScaleMap.value(currentRange);
        mc->setZoom(currentZoom);
//        calculateScale();
        loading = true;
    } else if(key == RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE ||
                  key == RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE )
    {
        updateMapCenterView();
    }
}

void Map::calculateScale()
{
    int m_current_zoom = mc->getCurrentZoom();
    double rangeRadar = m_radar_config->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toDouble();
    double rangeMap = scalebar_distances.at(m_current_zoom);
//    double rangeMap = currentRange;
    const double scalebar_line_length_px = rangeMap / std::pow(2.0, 18 - m_current_zoom) / 0.597164;
    const double radar_line_length_px = rangeRadar / RETURNS_PER_LINE;
    int iteration = scalebar_distances.size() - 1;

    for (int var = iteration; var >= 0 ; var--)
    {
        if(scalebar_distances.at(var) > currentRange)
        {
            rangeMap = scalebar_distances.at(var);
//            currentZoom = var;
//            currentScale = currentRange/rangeMap;
            break;
        }
    }
    currentScale = scalebar_line_length_px/radar_line_length_px;
}

void Map::updateMapView()
{
    qDebug()<<Q_FUNC_INFO<<"map queue size";
    qDebug()<<Q_FUNC_INFO<<mc->getMapQueueSize()<<"currentScale"<<currentScale<<"currentRange"<<currentRange<<"currentZoom calculated"<<currentZoom<<"currentZoom map"<<mc->getCurrentZoom()<<"currentZoom range"<<scalebar_distances.at(mc->getCurrentZoom());

    if((mc->getMapQueueSize() != 0) || loading)
    {
        if(mc->getMapQueueSize() > 0)
        {
            loading = true;
            qDebug()<<Q_FUNC_INFO<<"map loading...";
        }

        if(mc->getMapQueueSize() == 0)
        {
            loading = false;
            m_current_grab = mc->grab().toImage();

            QPixmap img = QPixmap::fromImage(m_current_grab);
            img.save(qApp->applicationDirPath()+QDir::separator()+"map_grab_before.png", "png");
            qDebug()<<Q_FUNC_INFO<<"image size before"<<m_current_grab.size();

            m_current_grab = m_current_grab.scaled(m_current_grab.size()*currentScale);

            qDebug()<<Q_FUNC_INFO<<"image size after"<<m_current_grab.size();
            img = QPixmap::fromImage(m_current_grab);
            img.save(qApp->applicationDirPath()+QDir::separator()+"map_grab_after.png", "png");

            m_text->load(m_current_grab);

            qDebug()<<Q_FUNC_INFO<<"map display change";
        }
    }

    /*
    if((mc->getMapQueueSize() != curLoadingMapSize) || loading)
    {
        if(mc->getMapQueueSize() != 0)
        {
            loading = true;
            qDebug()<<Q_FUNC_INFO<<"map loading...";
        }

        if(mc->getMapQueueSize() == 0 && curLoadingMapSize)
        {
            loading = false;
            m_current_grab = mc->grab().toImage();
            curLoadingMapSize = mc->getMapQueueSize();

            m_text->load(m_current_grab);

            qDebug()<<Q_FUNC_INFO<<"map display change";
        }
    }
    */
}

void Map::resize(const QSize &size)
{
    mc->setViewportSize(size);
    mc->resize(size);
}

void Map::drawInfo(QPainter* painter)
{
    QPen pen = painter->pen();
    QFont font = painter->font();

    if(loading)
    {
        int mode = 0; //temp map mode
        if(mode == 0)
            pen.setColor(Qt::yellow);
        else if(mode == 1)
            pen.setColor(Qt::black);

        font.setPixelSize(20);
        font.setBold(true);

        painter->setPen(pen);
        painter->setFont(font);

        painter->drawText(-50,-20,"Loading Map");
    }
}

void Map::drawTexture()
{
    vbo.create();
    vbo.bind();
    vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));

    m_environmentProgram->enableAttributeArray(0);
    m_environmentProgram->enableAttributeArray(1);
    m_environmentProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_environmentProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    m_environmentProgram->bind();
    m_text->bind();
    glDrawArrays(GL_TRIANGLE_FAN,0,4);
    m_text->unbind();
    vbo.release();
    m_environmentProgram->disableAttributeArray(0);
    m_environmentProgram->disableAttributeArray(1);
    m_environmentProgram->release();

}

void Map::onTimeout()
{
//    m_current_grab = mc->grab().toImage();
    /* test read and save from base64 image
    QPixmap img = QPixmap::fromImage(m_current_grab);
    img.save(qApp->applicationDirPath()+QDir::separator()+"map_grab.png", "png");
    */
//    m_text->load(m_current_grab);
    updateMapView();
}

void Map::updateMapCenterView()
{
    PointWorldCoord currentPosCenter(
                m_radar_config->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble(),
                m_radar_config->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble()
                );
    qreal lat_diff = fabs(currentPosCenter.latitude() - center.latitude());
    qreal lon_diff = fabs(currentPosCenter.longitude() - center.longitude());

    qDebug()<<Q_FUNC_INFO<<"lat diff"<<lon_diff<<"lon diff"<<lon_diff;

    if((lat_diff> 0.000001) || (lon_diff > 0.000001))
    {
        qDebug()<<Q_FUNC_INFO<<"map update center view";

        center.setLatitude(m_radar_config->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble());
        center.setLongitude(m_radar_config->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble());

        mc->setMapFocusPoint(center);
        loading = true;
    }
}


void Map::initGl()
{
    initializeOpenGLFunctions();
    m_vertexShader = new QGLShader(QGLShader::Vertex);
    m_vertexShader->compileSourceCode(vShader);

    m_fragmentShaders = new QGLShader(QGLShader::Fragment);
    m_fragmentShaders->compileSourceCode(fShader);

    m_environmentProgram = new QGLShaderProgram;
    m_environmentProgram->addShader(m_vertexShader);
    m_environmentProgram->addShader(m_fragmentShaders);
    m_environmentProgram->bindAttributeLocation("aPos", 0);
    m_environmentProgram->bindAttributeLocation("aTexCoord", 1);
    m_environmentProgram->link();
    m_environmentProgram->bind();
    m_environmentProgram->setUniformValue("texture1", 0);

    /*
        float vertices[4][8] = {
            {-1.f,  1.f, 0.0f,   0.25f, 0.25f},
            {-1.f, -1.f, 0.0f,   0.25f, .75f},
            {1.f, -1.f, 0.0f,  0.75f, 0.75f},
            {1.f,  1.f, 0.0f,  .75f, 0.25f}
        };
        */
    /*
        */
    float vertices[4][8] = {
        {-1.f,  1.f, 0.0f,   0.f, 0.f},
        {-1.f, -1.f, 0.0f,   0.f, 1.0f},
        {1.f, -1.f, 0.0f,  1.0f, 1.0f},
        {1.f,  1.f, 0.0f,  1.0f, 0.f}
    };
    for(int j = 0; j < 4; ++j)
    {
        vertData.append(vertices[j][0]);
        vertData.append(vertices[j][1]);
        vertData.append(vertices[j][2]);

        vertData.append(vertices[j][3]);
        vertData.append(vertices[j][4]);
    }

    m_text = new GLTextureCube(400,400);
}

GLTexture::GLTexture() : m_texture(0), m_failed(false)
{
    glGenTextures(1, &m_texture);
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &m_texture);
}

GLTextureCube::GLTextureCube(int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTextureCube::load(const QImage &img)
{
    // TODO: Add error handling.

    glBindTexture(GL_TEXTURE_2D, m_texture);

    QImage image(img);

    if (image.isNull()) m_failed = true;

    image = image.convertToFormat(QImage::Format_ARGB32);

    //    qDebug() << "Image size:" << image.width() << "x" << image.height();

    glTexImage2D(GL_TEXTURE_2D, 0, 4, image.width(), image.height(), 0,
                 GL_BGRA, GL_UNSIGNED_BYTE, image.bits());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTextureCube::bind()
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glEnable(GL_TEXTURE_2D);
}

void GLTextureCube::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}
