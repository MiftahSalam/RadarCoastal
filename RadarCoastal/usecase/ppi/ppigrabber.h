#ifndef PPIGRABBER_H
#define PPIGRABBER_H

#include <RadarEngine/radarconfig.h>

#include <QObject>

class PPIGrabber : public QObject
{
    Q_OBJECT
public:
    PPIGrabber(PPIGrabber& other) = delete;
    void operator=(const PPIGrabber&) = delete;

    static PPIGrabber* GetInstance(QObject *parent = nullptr);

    void start();
    void stop();
    void update();
    void grab(QImage image);
    bool pendingGrabAvailable() const;
    bool isStart() const;

protected:
    explicit PPIGrabber(QObject *parent = nullptr);

signals:
    void signalSendEcho(const QString echo, const int vp_width, const int vp_height);

private slots:
    void trigger_radarConfigChange(QString key, QVariant val);

private:
    RadarEngine::RadarConfig* m_instance_cfg;
    static PPIGrabber* m_grabber;

    void stateChange(int state);

    int currentAngle;
    bool grabStart;
    bool grabPending;
};

#endif // PPIGRABBER_H
