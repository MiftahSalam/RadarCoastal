#ifndef UTILS_H
#define UTILS_H

#include <QPointF>
#include <QStringList>

class Utils
{
public:
    static const QString MtrUnitStr;
    static const QString KmUnitStr;
    static const QString NMUnitStr;

    static quint8 unit;

    static QPointF PixToGPS(const int pos_x, const int pos_y, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon);
    static QPointF GPSToPix(const double lon, const double lat, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon);
    static QPointF DistancePolar(const int pos_x, const int pos_y, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon);
    static qreal DistanceFromCenterInPix(const qreal distance_in_km, const int vp_width, const int vp_height, const double vp_range);
    static QStringList GPSString(const double lon, const double lat);
    static QString TickToTime(quint8 tick);
    static QPointF GpsAbsolute(double lat, double lon);
};


#endif // UTILS_H
