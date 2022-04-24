#include "utils.h"

#include <QLineF>

#include <radarconfig_global.h>

QPointF pixToGPS(const int pos_x, const int pos_y, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon)
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

    //    qDebug()<<Q_FUNC_INFO<<"line"<<line.length()<<angle<<line;
    //    qDebug()<<Q_FUNC_INFO<<"pol"<<pol.r<<pol.angle;
    //    qDebug()<<Q_FUNC_INFO<<"pos"<<pos.lat<<pos.lon<<currentRange;


    QPointF pos_to_convert;
    pos_to_convert.setX(lon);
    pos_to_convert.setY(lat);

    return pos_to_convert;
}

QPointF GPSToPix(const double lon, const double lat, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon)
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

    qDebug()<<Q_FUNC_INFO<<"line"<<line.length()<<angle<<line;
    qDebug()<<Q_FUNC_INFO<<"pol"<<r_radar<<angle_radar;
    qDebug()<<Q_FUNC_INFO<<"pos"<<lat<<lon<<vp_range;


    QPointF pos_to_convert;
    //    double dif_lat = (lat - currentOwnShipLat) * currentRange;
    //    double dif_lon = (lon - currentOwnShipLon) * cos(deg2rad(currentOwnShipLat)) * currentRange;
    //    pos.x = static_cast<int>(dif_lon);
    //    pos.y = static_cast<int>(dif_lat);

    pos_to_convert.setX(line.p2().x());
    pos_to_convert.setY(line.p2().y());

    return pos_to_convert;
}

QPointF distancePolar(const int pos_x, const int pos_y, const int vp_width, const int vp_height, const double vp_range, const double own_lat, const double own_lon)
{
    QPoint screen_middle(vp_width/2,vp_height/2);
    QPointF gps = pixToGPS(pos_x-screen_middle.x(), -pos_y+screen_middle.y(),vp_width,vp_height,vp_range,own_lat,own_lon);
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

QStringList GPSString(const double lon, const double lat)
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
    min = lat_ - deg ;
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
