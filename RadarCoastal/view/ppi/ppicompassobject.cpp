#include "ppicompassobject.h"
#include "qmath.h"

PPICompassObject::PPICompassObject(QObject *parent): PPIObject(parent)
{
}

void PPICompassObject::Draw(QPainter* painter, const int &side)
{
    const bool show_compass = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_SHOW_COMPASS).toBool();
    if(show_compass)
    {
        const bool heading_up = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_PPI_DISPLAY_HEADING_UP).toInt();
        const double bearing = RadarEngine::RadarConfig::getInstance("")->getConfig(RadarEngine::NON_VOLATILE_NAV_DATA_LAST_HEADING).toDouble();

        QPen curPen = painter->pen();

        /*
          compass ring text
        */
        int brnCor = 0;
        int brn;
        double buf_j;
        int margin_a = 5;
        int margin_b = 15;

        if(heading_up)
            brnCor =  -static_cast<int>(bearing);

        QString text;
        for(int j=0;j<12;j++)
        {
            /*
            brn = (j*30)+180;
            brn = brn == 360 ? 0 : brn;

            while(brn>360 || brn<0)
            {
                if(brn>360)
                    brn -= 360;
                if(brn<0)
                    brn += 360;
            }
            */

            if(j<9)
                brn = (j*30)+90;
            else
                brn = (j*30)-270;

            QTextStream(&text)<<brn;

            buf_j = (j*30)+brnCor;
            /*
            */
            while(buf_j>360 || buf_j<0)
            {
                if(buf_j>360)
                    buf_j -= 360;
                if(buf_j<0)
                    buf_j += 360;
            }

            double x = static_cast<double>(side-20)*cos((buf_j)*M_PI/180);
            double y = static_cast<double>(side-20)*sin((buf_j)*M_PI/180);
            QRect rect(static_cast<int>(x)-margin_b,static_cast<int>(y)-margin_a,30,15);
            QTextOption opt;
            opt.setAlignment(Qt::AlignHCenter);
            QFont font;

            font.setPixelSize(12);
            painter->setFont(font);
            painter->drawText(rect,text,opt);
            text.clear();
        }

        /*
          compass ring
        */
        if(heading_up)
            painter->rotate(-bearing);

        for(int j=0;j<180;j++)
        {
            margin_a = 10;
            margin_b = 5;

            if(j%15==0)
                painter->drawLine(0,side,0,side-margin_a);
            else
                painter->drawLine(0,side,0,side-margin_b);

            painter->rotate(2);
        }

        if(heading_up)
            painter->rotate(bearing);

        painter->setPen(curPen);
    }

}
