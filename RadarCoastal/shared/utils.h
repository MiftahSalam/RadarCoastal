#ifndef UTILS_H
#define UTILS_H

#include <QDateTime>
#include <QPointF>
#include <QStringList>

class Utils
{
public:
    static const QString MtrUnitStr;
    static const QString KmUnitStr;
    static const QString NMUnitStr;

    static const int ONE_PRECISION;
    static const int TWO_PRECISION;
    static const int FIVE_PRECISION;

    static quint8 unit;

    static QPointF PixToGPS(const int pos_x, const int pos_y, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon);
    static QPointF GPSToPix(const double lon, const double lat, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon);
    static QPointF DistancePolar(const int pos_x, const int pos_y, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon);
    static qreal DistanceFromCenterInPix(const qreal distance_in_km, const int vp_width, const int vp_height, const double vp_range);
    static QStringList GPSString(const double lon, const double lat);
    static QPointF GpsAbsolute(double lat, double lon);

    static QString TickToTime(quint8 tick);
    static QString TimeElapsedDisplay(qint64 elapsedSecond);
    static QDateTime TimeElapsed(qint64 elapsedSecond);
};


#endif // UTILS_H
