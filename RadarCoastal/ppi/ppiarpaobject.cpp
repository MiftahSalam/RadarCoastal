#include "ppiarpaobject.h"

PPIArpaObject::PPIArpaObject(QObject *parent): PPIObject(parent)
{
    m_re = RadarEngine::RadarEngine::getInstance();
}

void PPIArpaObject::draw(QPainter* painter)
{
    const int preset_color = RadarConfig::RadarConfig::getInstance("")->getConfig(RadarConfig::VOLATILE_DISPLAY_PRESET_COLOR).toInt();

    if(preset_color == 0)
        painter->setPen(QColor(255,255,255,255));
    else if(preset_color == 1)
        painter->setPen(QColor(0,0,0,255));


    painter->drawLine(20,20,40,40);


    if(preset_color == 0)
        painter->setPen(QColor(0,255,0,255));
    else if(preset_color == 1)
        painter->setPen(QColor(255,255,0,255));

}

void PPIArpaObject::createMARPA(QPoint pos)
{

}
