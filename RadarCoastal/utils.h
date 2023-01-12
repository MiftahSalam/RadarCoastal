#ifndef UTILS_H
#define UTILS_H

#include <QPointF>
#include <QStringList>

extern QPointF pixToGPS(const int pos_x, const int pos_y, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon);
extern QPointF GPSToPix(const double lon, const double lat, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon);
extern QPointF distancePolar(const int pos_x, const int pos_y, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon);
extern qreal distanceFromCenterInPix(const qreal distance_in_km, const int vp_width, const int vp_height, const double vp_range);
extern QStringList GPSString(const double lon, const double lat);
extern QString tickToTime(quint8 tick);
extern QPointF gpsAbsolute(double lat, double lon);

#endif // UTILS_H
