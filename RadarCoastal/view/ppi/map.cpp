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
    m_radar_config = RadarEngine::RadarConfig::getInstance("");

    mc = new QMapControl(QSizeF(400,400));
    mc->enablePersistentCache();
    mc->enableZoomControls(false);

    std::shared_ptr<LayerMapAdapter> mapLayer(std::make_shared<LayerMapAdapter>("layer view"));
    mapLayer->setMapAdapter(std::make_shared<MapAdapterGoogle>(MapAdapterGoogle::GoogleLayerType::SATELLITE_ONLY));

    mc->addLayer(mapLayer);

    PointWorldCoord center(
                m_radar_config->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble(),
                m_radar_config->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble()
                );
    mc->setMapFocusPoint(center);
    mc->setViewportSize(QSizeF(400,400));
    mc->setZoom(7);
//    mc->show();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Map::onTimeout);
    timer->start(3000);
}

void Map::resize(const QSize &size)
{
    mc->setViewportSize(size);
    mc->resize(size);
    m_current_grab = mc->grab().toImage();
}

void Map::drawTexture()
{
    m_text->load(m_current_grab);

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
    m_current_grab = mc->grab().toImage();
    /* test read and save from base64 image
    QPixmap img = QPixmap::fromImage(m_current_grab);
    img.save(qApp->applicationDirPath()+QDir::separator()+"map_grab.png", "png");
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
