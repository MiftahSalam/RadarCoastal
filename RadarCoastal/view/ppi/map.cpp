#include "map.h"

#include <QMapControl/LayerMapAdapter.h>
#include <QMapControl/MapAdapterGoogle.h>

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

Map::Map(QObject *parent)
    : QObject{parent}
{
    loading = true;

    m_radar_config = RadarEngine::RadarConfig::getInstance("");
    connect(m_radar_config,&RadarEngine::RadarConfig::configValueChange,
            this,&Map::triggerConfigChange);

    mc = new QMapControl(QSizeF(400,400));
    mc->enablePersistentCache();
    mc->enableZoomControls(false);

    std::shared_ptr<LayerMapAdapter> mapLayer(std::make_shared<LayerMapAdapter>("layer view"));
    mapLayer->setMapAdapter(std::make_shared<MapAdapterGoogle>(MapAdapterGoogle::GoogleLayerType::SATELLITE_ONLY));

    mc->addLayer(mapLayer);

    center.setLatitude(m_radar_config->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble());
    center.setLongitude(m_radar_config->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble());
    currentRange = m_radar_config->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE).toInt();

    mc->setMapFocusPoint(center);
    mc->setViewportSize(QSizeF(400,400));
    mc->setZoom(7);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Map::onTimeout);
    timer->start(1000);
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

void Map::triggerConfigChange(const QString key, const QVariant val)
{
    if(key == RadarEngine::NON_VOLATILE_PPI_DISPLAY_LAST_SCALE)
    {
        int range = val.toInt();
        if(range < currentRange) mc->zoomIn(); //temp
        else if(range > currentRange) mc->zoomOut(); //temp

        currentRange = range;
        loading = true;
    } else if(key == RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE ||
                  key == RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE )
    {
        updateMapCenterView();
    }
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

void Map::updateMapView()
{
    qDebug()<<Q_FUNC_INFO<<mc->getMapQueueSize();

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
