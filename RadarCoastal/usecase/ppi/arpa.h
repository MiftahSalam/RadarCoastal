#ifndef PPIARPA_H
#define PPIARPA_H

#include <RadarEngine/radarengine.h>
#include <RadarEngine/radarconfig.h>

class PPIArpa : public QObject
{
    Q_OBJECT
public:
    PPIArpa(QObject* parent = nullptr, RadarEngine::RadarEngine* re = nullptr, RadarEngine::RadarConfig* config = nullptr);

    RadarEngine::RadarEngine* m_re;

public slots:
    void createMARPA(const QPoint &pos, const int vp_width, const int vp_height);

private:
    RadarEngine::RadarConfig* m_config_instance;
};

#endif // PPIARPA_H
