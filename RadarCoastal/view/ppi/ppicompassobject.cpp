#include "ppicompassobject.h"
#include "shared/config/applicationconfig.h"

#include <qmath.h>

PPICompassObject::PPICompassObject(QObject *parent): PPIObject(parent)
{
}

void PPICompassObject::Draw(QPainter* painter, const int &side, const int &width, const int &height, const QPoint &off_center)
{
    QPoint center_point = QPoint(width/2,height/2);

    const bool show_compass = ApplicationConfig::getInstance()->getPpiConfig()->getShowCompass();
    if(show_compass)
    {
        painter->translate(-off_center);
        painter->translate(center_point);

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


            QPoint p1(-center_point+off_center);
            //                QPoint p1(-off_center.x(),-off_center.y());
            //            QPoint p1(-height/4,-height/4);
            //            QPoint p1(0,-height/4);
            QPoint p2(side*qCos(j*2*M_PI/180.),side*qSin(j*2*M_PI/180.));
            QLineF line1(p1,p2);
            QLineF line2;
            //            QPoint p3 = line1.pointAt(0.95).toPoint();

            line2.setP1(p2);
            line2.setAngle(line1.angle());
            //            line2.setLength(-10);
            //            qDebug()<<Q_FUNC_INFO<<"j"<<j<<"p2"<<p2;
            //            qDebug()<<Q_FUNC_INFO<<"line2"<<line2;

            if(j%15==0)
            {
                line2.setLength(-margin_a);
                painter->drawLine(line2);
            }
            //                painter->drawLine(p2,p3);
            //                painter->drawLine(0,-side,p3.x(),p3.y());
            //                painter->drawLine(0,-side,0,-side+margin_a);
            //            painter->drawLine(0,side,0,side-margin_a);
            else
            {
                line2.setLength(-margin_b);
                painter->drawLine(line2);
            }
            //                painter->drawLine(p2,p3);
            //                painter->drawLine(0,-side,p3.x(),p3.y());
            //                painter->drawLine(0,-side,0,-side+margin_b);
            //            painter->drawLine(0,side,0,side-margin_b);
        }

        //        for(int j=0;j<180;j++)
        //        {
        //            margin_a = 10;
        //            margin_b = 5;

        //            if(j%15==0)
        //                painter->drawLine(0,side,0,side-margin_a);
        //            else
        //                painter->drawLine(0,side,0,side-margin_b);

        //            painter->rotate(2);
        //        }

        if(heading_up)
            painter->rotate(bearing);

        painter->setPen(curPen);

        painter->translate(-center_point); //tes offenter
        painter->translate(off_center); //tes offcenter
    }
}

QPoint PPICompassObject::calculateGradien(const QPoint &p1, const QPoint &p2)
{
    QLineF line1(p1,p2);
//    float gradient = line1.dy()/line1.dx();
//    float c = line1.y1()-(gradient*line1.x1());

    // r2 =

//    qDebug()<<Q_FUNC_INFO<<"dx"<<line1.dx()<<"dy"<<line1.dy()<<"gradient"<<gradient<<"c"<<c<<"angle"<<line1.angle()<<"point parameter"<<line1.pointAt(0.9);

    return line1.pointAt(0.5).toPoint();
}

