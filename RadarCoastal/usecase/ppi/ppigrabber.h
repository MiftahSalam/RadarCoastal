#ifndef PPIGRABBER_H
#define PPIGRABBER_H

#include <RadarEngine/radarconfig.h>

#include <QObject>

class PPIGrabber : public QObject
{
    Q_OBJECT
public:
    explicit PPIGrabber(QObject *parent = nullptr);

    void start();
    void stop();
    void update();
    void grab(QImage image);
    bool pendingGrabAvailable() const;
    bool isStart() const;

signals:

private slots:
    void trigger_radarConfigChange(QString key, QVariant val);

private:
    RadarEngine::RadarConfig* m_instance_cfg;

    void stateChange(int state);

    int currentAngle;
    bool grabStart;
    bool grabPending;
};

#endif // PPIGRABBER_H
