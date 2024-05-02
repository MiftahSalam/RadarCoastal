#include "shared/utils.h"

#include <QLineF>
#include <qmath.h>

#include <RadarEngine/global.h>
#include <RadarEngine/radarconfig.h>

#ifdef USE_LOG4QT
#include <log4qt/logger.h>
LOG4QT_DECLARE_STATIC_LOGGER(logger, Utils)
#else
#include <QDebug>
#endif

const QString Utils::MtrUnitStr = " m";
const QString Utils::KmUnitStr = " Km";
const QString Utils::NMUnitStr = " NM";

const int Utils::ONE_PRECISION = 1;
const int Utils::TWO_PRECISION = 2;
const int Utils::FIVE_PRECISION = 5;

quint8 Utils::unit = 0;

char Utils::degChar = static_cast<char>(176);

QPointF Utils::PixToGPS(const int pos_x, const int pos_y, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon)
{
    QPoint screen_middle(0,0);
    QPointF event_pos_scaled(pos_x,pos_y);
    QLineF line(screen_middle, event_pos_scaled);
    double r_radar,angle_radar,lat,lon;
    double angle = line.angle()+90.;
    //    const int MAX_PIX = qMin(vp_width/2,vp_height/2)-20; //tes
    const int MAX_PIX = qMin(vp_width/2,vp_height/2);

    while (angle >=360. || angle < 0. ) {
        if(angle >= 360.)
            angle -= 360.;
        if(angle < 0.)
            angle += 360.;
    }

    angle_radar = SCALE_DEGREES_TO_RAW2048(angle);
    r_radar = static_cast<int>(line.length());
    lat = own_lat +
            static_cast<double>(r_radar) / static_cast<double>(MAX_PIX) * vp_range * cos(deg2rad(SCALE_RAW_TO_DEGREES2048(angle_radar))) / 60. / 1852.;
    lon = own_lon +
            static_cast<double>(r_radar) / static_cast<double>(MAX_PIX) * vp_range * sin(deg2rad(SCALE_RAW_TO_DEGREES2048(angle_radar))) /
            cos(deg2rad(own_lat)) / 60. / 1852.;

#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO<<"line"<<line.length()<<angle;
#endif
//        qDebug()<<Q_FUNC_INFO<<"line"<<line.length()<<angle<<line;
//        qDebug()<<Q_FUNC_INFO<<"pol"<<pol.r<<pol.angle;
//        qDebug()<<Q_FUNC_INFO<<"pos"<<pos.lat<<pos.lon<<currentRange;


    QPointF pos_to_convert;
    pos_to_convert.setX(lon);
    pos_to_convert.setY(lat);

    return pos_to_convert;
}

QPointF Utils::GPSToPix(const double lon, const double lat, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon)
{
    QPoint screen_middle(0,0);
    QPointF event_pos_scaled(1,1);
    QLineF line(screen_middle,event_pos_scaled);
    double r_radar,angle_radar;
    double angle = line.angle()+90.;
    //    Position own_pos;
    //    Position pos;
    //    double angle;
    double dif_lat = lat;
    double dif_lon;
    //    const int MAX_PIX = qMin(width()/2,height()/2)-20; //tes
    const int MAX_PIX = qMin(vp_width/2,vp_height/2);

    //    own_pos.lat = currentOwnShipLat;
    //    own_pos.lon = currentOwnShipLon;
    //    pos.lat = lat;
    //    pos.lon = lon;
    dif_lat -= own_lat;
    dif_lon = (lon - own_lon) * cos(deg2rad(own_lat));
    r_radar = static_cast<int>((sqrt(dif_lat * dif_lat + dif_lon * dif_lon) * 60. * 1852. * static_cast<double>(MAX_PIX) / vp_range + 1));
    angle_radar = static_cast<int>((atan2(dif_lon, dif_lat)) * static_cast<double>(LINES_PER_ROTATION) / (2. * M_PI) + 1);  // + 1 to minimize rounding errors
    if (angle_radar < 0) angle_radar += LINES_PER_ROTATION;

    angle = SCALE_RAW_TO_DEGREES2048(angle_radar)-90.;

    while (angle >=360. || angle < 0. ) {
        if(angle >= 360.)
            angle -= 360.;
        if(angle < 0.)
            angle += 360.;
    }

    line.setAngle(angle);
    line.setLength(r_radar);

#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO<<"line"<<line.length()<<angle;
    logger()->trace()<<Q_FUNC_INFO<<"pol"<<r_radar<<angle_radar;
    logger()->trace()<<Q_FUNC_INFO<<"pos"<<lat<<lon<<vp_range;
#else
    qDebug()<<Q_FUNC_INFO<<"line"<<line.length()<<angle<<line;
    qDebug()<<Q_FUNC_INFO<<"pol"<<r_radar<<angle_radar;
    qDebug()<<Q_FUNC_INFO<<"pos"<<lat<<lon<<vp_range;
#endif

    QPointF pos_to_convert;
    //    double dif_lat = (lat - currentOwnShipLat) * currentRange;
    //    double dif_lon = (lon - currentOwnShipLon) * cos(deg2rad(currentOwnShipLat)) * currentRange;
    //    pos.x = static_cast<int>(dif_lon);
    //    pos.y = static_cast<int>(dif_lat);

    pos_to_convert.setX(line.p2().x());
    pos_to_convert.setY(line.p2().y());

    return pos_to_convert;
}

QPointF Utils::DistancePolar(const int pos_x, const int pos_y, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon)
{
    QPoint screen_middle(vp_width/2,vp_height/2);
    QPointF gps = Utils::PixToGPS(pos_x-screen_middle.x(), -pos_y+screen_middle.y(),vp_width,vp_height,vp_range,own_lat,own_lon);
    double const R = 6371.;

    double dif_lat = deg2rad(gps.y());
    double dif_lon = (deg2rad(gps.x()) - deg2rad(own_lon))
            * cos(deg2rad((own_lat+gps.y())/2.));
    dif_lat =  dif_lat - (deg2rad(own_lat));

    double km = sqrt(dif_lat * dif_lat + dif_lon * dif_lon)*R;
    double bearing = atan2(dif_lon,dif_lat)*180./M_PI;

    while(bearing < 0.0)
    {
        bearing += 360.0;
    }

    return QPointF(km,bearing);
}

qreal Utils::DistanceFromCenterInPix(const qreal distance_in_km, const int vp_width, const int vp_height, const double vp_range)
{
    double r_radar_pix;
    const int MAX_PIX = qMin(vp_width/2,vp_height/2);
    r_radar_pix = static_cast<int>((distance_in_km * static_cast<double>(MAX_PIX) / vp_range));

    //    qDebug()<<Q_FUNC_INFO<<"r_radar_pix"<<r_radar_pix;

    return r_radar_pix;
}

QStringList Utils::GPSString(const double lon, const double lat)
{
    QString lat_hemis,lon_hemis;

    if(lon < 0) lon_hemis = "W";
    else lon_hemis = "E";
    if(lat < 0) lat_hemis = "S";
    else lat_hemis = "N";

    double lat_ = abs(lat);
    double deg = floor(lat_);
    double min = lat_ - deg ;
    min = min * 60;
    min = floor(min);
    double sec = (lat_ - deg - (min / 60.0)) * 3600.0;

    QString deg_string = QString::number(deg,'f', 0);
    if (deg_string.size() < 2)
        deg_string.prepend("0");

    QString min_string = QString::number(min,'f', 0);
    if (min_string.size() < 2)
        min_string.prepend("0");

    QString sec_string = QString::number(sec, 'f', 0);
    if (sec_string.size() <2)
        sec_string.prepend("0");

    QString latitude_string = deg_string + "-" + min_string + "'" + sec_string + "''" + lat_hemis;


    double lon_ = abs(lon);
    deg = floor(lon_);
    min = lon_ - deg ;
    min = min * 60;
    min = floor(min);
    sec = (lon_ - deg - (min / 60.0)) * 3600.0;

    deg_string = QString::number(deg,'f', 0);
    if (deg_string.size() < 2)
        deg_string.prepend("00");
    else if (deg_string.size() < 3)
        deg_string.prepend("0");

    min_string = QString::number(min,'f', 0);
    if (min_string.size() < 2)
        min_string.prepend("0");

    sec_string = QString::number(sec, 'f', 0);
    if (sec_string.size() <2)
        sec_string.prepend("0");

    QString longitude_string = deg_string + "-" + min_string + "'" + sec_string + "''" + lon_hemis;

    return QStringList()<<latitude_string<<longitude_string;
}

void Utils::GPSDMM2Dec(QString lonStr, QString latStr, double *lon, double *lat)
{
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
    auto lonList = lonStr.split(",", Qt::SkipEmptyParts);
    auto latList = latStr.split(",", Qt::SkipEmptyParts);
#else
    auto lonList = lonStr.split(",", QString::SkipEmptyParts);
    auto latList = latStr.split(",", QString::SkipEmptyParts);
#endif
    *lon = lonList.at(0).toDouble()/100.;
    *lat = latList.at(0).toDouble()/100.;

    auto deg = floor(*lon);
    auto min = (*lon - deg)*100.;
    min = min / 60;
    *lon = deg + min;

    deg = floor(*lat);
    min = (*lat - deg)*100.;
    min = min / 60;
    *lat = deg + min;

    if (lonList.at(1) == "W")
        *lon *= -1;

    if (latList.at(1) == "S")
        *lat *= -1;
}

QString Utils::TickToTime(quint8 tick)
{
    //    qDebug()<<Q_FUNC_INFO<<"tick"<<tick;

    QString sec, min;
    if(tick<60)
    {
        sec = QString::number(tick);
        if(sec.size() < 2) sec.prepend("0");
        min = "00";
    }
    else
    {
        int sec_int = tick % 60;
        int min_int = tick / 60;

        sec = QString::number(sec_int);
        min = QString::number(min_int);

        if(sec.size() < 2) sec.prepend("0");
        if(min.size() < 2) min.prepend("0");
    }

    return min+":"+sec;
}

QPointF Utils::GpsAbsolute(double lat, double lon)
{
    RadarEngine::RadarConfig* instance = RadarEngine::RadarConfig::getInstance("");
    double dlon = 0.;
    double dlat = 0.;
    if(instance->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toBool())
    {
        double hdt = instance->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();
        double mid_lat = instance->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LATITUDE).toDouble();
        double mid_lon = instance->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_LONGITUDE).toDouble();
        double offset_lon = lon-mid_lon;
        double offset_lat = lat-mid_lat;
        double cos_angle = qCos(hdt*M_PI/180);
        double sin_angle = qSin(hdt*M_PI/180);
        dlon = offset_lon*cos_angle-offset_lat*sin_angle;
        dlat = offset_lon*sin_angle+offset_lat*cos_angle;

#ifdef USE_LOG4QT
    logger()->trace()<<Q_FUNC_INFO;
    logger()->trace()<<Q_FUNC_INFO<<"offset_lon"<<offset_lon;
    logger()->trace()<<Q_FUNC_INFO<<"offset_lat"<<offset_lat;
    logger()->trace()<<Q_FUNC_INFO<<"cos_angle"<<cos_angle;
    logger()->trace()<<Q_FUNC_INFO<<"sin_angle"<<sin_angle;
    logger()->trace()<<Q_FUNC_INFO<<"dlon"<<dlon;
    logger()->trace()<<Q_FUNC_INFO<<"dlat"<<dlat;
#endif
    }

    return QPointF(lon-dlon,lat-dlat);
}

QString Utils::TimeElapsedDisplay(qint64 elapsedSecond)
{
    QDateTime from = QDateTime::fromSecsSinceEpoch(0);
    QDateTime to = QDateTime::fromSecsSinceEpoch(elapsedSecond);
    QDate dFrom = from.date();
    QTime tFrom = from.time();
    QDate dTo = to.date();
    QTime tTo = to.time();
    int dy = dTo.year()-dFrom.year();
    int dm = dTo.month()-dFrom.month();
    int dd = dTo.day()-dFrom.day();
    int dh = tTo.hour()-tFrom.hour();
    int dmin = tTo.minute()-tFrom.minute();
    int ds = tTo.second()-tFrom.second();

    QString yStr = QString::number(dy);
    QString mStr = QString::number(dm);
    QString dStr = QString::number(dd);
    QString hStr = QString::number(dh);
    QString minStr = QString::number(dmin);
    QString sStr = QString::number(ds);

    return yStr+" y, "+mStr+" m, "+dStr+" d, "+hStr+" h, "+minStr+" min, "+sStr+" s";
}

QDateTime Utils::TimeElapsed(qint64 elapsedSecond)
{
    QDateTime from = QDateTime::fromSecsSinceEpoch(0);
    QDateTime to = QDateTime::fromSecsSinceEpoch(elapsedSecond);
    QDate dFrom = from.date();
    QTime tFrom = from.time();
    QDate dTo = to.date();
    QTime tTo = to.time();
    QDateTime res;

    int dy = dTo.year()-dFrom.year();
    int dm = dTo.month()-dFrom.month();
    int dd = dTo.day()-dFrom.day();
    int dh = tTo.hour()-tFrom.hour();
    int dmin = tTo.minute()-tFrom.minute();
    int ds = tTo.second()-tFrom.second();

    res.setDate(QDate(dy, dm, dd));
    res.setTime(QTime(dh, dmin, ds));

    return res;
}

QString Utils::RangeDisplay(double rng, int precision)
{
    QString unit_str;
    QString rngStr;
    switch (unit) {
    case 0:
        break;
    default:
        break;
    }

    if(rng >= 1000.)
    {
        rng /= 1000.;
        switch (unit) {
        case 0:
            unit_str = Utils::KmUnitStr;
            break;
        case 1:
            rng *= KM_TO_NM;
            unit_str = Utils::NMUnitStr;
            break;
        default:
            break;
        }
        rngStr = QString::number(rng,'f',precision)+unit_str;
    }
    else
    {
        rngStr = QString::number(static_cast<int>(rng))+Utils::MtrUnitStr;
    }

    return rngStr;
}
