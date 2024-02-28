#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QOpenGLFunctions>

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
    void drawTexture();

    QImage m_current_grab;

signals:

private slots:
    void onTimeout();

private:
    QMapControl* mc;
    QTimer *timer;
    RadarEngine::RadarConfig* m_radar_config;

    void initGl();
};

#endif // MAP_H
