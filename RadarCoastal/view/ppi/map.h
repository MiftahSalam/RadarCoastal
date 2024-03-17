#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QGLBuffer>
#include <QGLShaderProgram>
#include <QGLShader>

#include <QMapControl/QMapControl.h>

#include <RadarEngine/radarconfig.h>

using namespace qmapcontrol;


class GLTexture
{
public:
    GLTexture();
    virtual ~GLTexture();
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual bool failed() const {return m_failed;}
protected:
    GLuint m_texture;
    bool m_failed;
};

class GLTextureCube : public GLTexture
{
public:
    explicit GLTextureCube(int width, int height);
    void load(const QImage &img);
    void bind();
    void unbind();
};


class Map : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit Map(QObject *parent = nullptr);

    void resize(const QSize &size);
    void drawInfo(QPainter* painter);
    void drawTexture();
    void initGl();
    bool isLoading() const { return loading; }

signals:

private slots:
    void onTimeout();
    void triggerConfigChange(const QString key, const QVariant val);

private:
    QMapControl* mc;
    PointWorldCoord center;
    QTimer *timer;
    RadarEngine::RadarConfig* m_radar_config;
    QGLBuffer vbo;
    QGLShader *m_vertexShader;
    QGLShader *m_fragmentShaders;
    QGLShaderProgram *m_environmentProgram;
    GLTextureCube *m_text;
    QVector<GLfloat> vertData;
    QImage m_current_grab;
    int currentRange;
    bool loading;

    void updateMapView();
    void updateMapCenterView();
};

#endif // MAP_H
