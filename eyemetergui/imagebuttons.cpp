#include "imagebuttons.h"
#include <QDebug>
ImageButtons::ImageButtons(QWidget *parent) :
    QWidget(parent)
{
    qDebug()<<Q_FUNC_INFO;

    d_but_prev = new QPushButton(parent);
    //d_but_prev->setStyleSheet("border-width: 5px; border-color: yellow;");
    //d_but_prev->setStyleSheet(" background-color: yellow; border-radius: 10px; width: 60px; height: 60px;");
//                              "border-width: 2px;"
//                             " border-radius: 10px;"
//                              "border-color: beige;");
    //d_but_prev->setIcon(QIcon(":/img/prev1.png"));
    //d_but_prev->setIcon(QIcon(":/img/prev.png"));
    d_but_prev->setStyleSheet("QPushButton { "//"color: grey;"
                              //"border-image: url(:/img/prev.png) 5 5 5 5 ;"
                              "border-image: url(:/img/angle-left.svg) ;"
                              "background: none;"
                              //"border-image: url(:/img/angle-circle-left.svg) 0 0 0 0;"
                              //"border-radius: 10px;"
                              "border-top: transparent;"
                              "border-bottom: transparent;"
                              "border-right: transparent;"
                              "border-left: transparent;}"
                              "QPushButton::disabled { "
                              "border-image: url(:/img/angle-left_disabled.svg) ;"
                              "}"
                              );
    //d_but_prev->setStyleSheet("border: none");
    d_but_prev->setIconSize(QSize(shortSide, longSide));
    connect(d_but_prev, SIGNAL(clicked()), SLOT(slot_imagePrev()));

    d_but_next = new QPushButton(parent);
    //d_but_next->setIcon(QIcon(":/img/next.png"));
    d_but_next->setStyleSheet("QPushButton { "//"background-color: #414447;"
                              //"border-image: url(:/img/next.png) 50 50 50 50;"
                              "border-image: url(:/img/angle-right.svg) 5 5 5 5;"
                              "background: none;"
                              "border-radius: 10px;"
                              "border-top: transparent;"
                              "border-bottom: transparent;"
                              "border-right: transparent;"
                              "border-left: transparent;}"
                              "QPushButton::disabled { "
                              "border-image: url(:/img/angle-right_disabled.svg) ;"
                              "}"
                              //"border-margin: 20 20 20 20;"
                              );

    //d_but_next->setStyleSheet("border: none");
    //d_but_next->setBaseSize(150,150);
    d_but_next->setIconSize(QSize(shortSide, longSide));
    connect(d_but_next, SIGNAL(clicked()), SLOT(slot_imageNext()));
#ifdef TEST_snapshot
    //d_but_next->setDisabled(true);
        d_but_prev->setDisabled(true);
#endif
    //resize(parent->width(), parent->height());
//    d_but_prev->setStyleSheet(" color: white;"
//                  "background-color: #27a9e3;"
//                  "border-width: 0px;"
//                 " border-radius: 30px;  ");
}

void ImageButtons::hide(bool visible)
{
    qDebug()<<Q_FUNC_INFO << visible;
//    if(!visible)
//    {
//        d_but_prev->show();
//        d_but_next->show();
//    } else
//    {
//        d_but_prev->hide();
//        d_but_next->hide();
//    }
    d_but_prev->setVisible(!visible);
    d_but_next->setVisible(!visible);

}

void ImageButtons::setImageCount(uint imageCount)
{
    qDebug()<<Q_FUNC_INFO << imageCount;
    d_imageNum = imageCount;
    d_imageCount = imageCount;
#ifndef TEST_snapshot

#endif
    if(imageCount < 2)
    {
        qDebug()<<Q_FUNC_INFO << "imageCount < 2";
//        d_but_prev->setDisabled(true);
//        d_but_next->setDisabled(true);
        d_but_prev->setVisible(false);
        d_but_next->setVisible(false);
    } else
//        #endif
    {
//        if(imageCount == 2)
//        {
////            d_but_prev->setEnabled(true);
////            d_but_next->setDisabled(true);
//            d_but_prev->setVisible(true);
//            d_but_next->setVisible(false);
//        } else
        {
            qDebug()<<Q_FUNC_INFO << "imageCount >= 2";
//            d_but_prev->setEnabled(true);
//            d_but_next->setEnabled(true);
            d_but_prev->setVisible(true);
            d_but_next->setVisible(true);
            qDebug()<<Q_FUNC_INFO << "imageCount >= 2 end";
        }
    }
    //d_but_prev->setEnabled(true);
    //d_but_next->setDisabled(true);
    d_but_prev->setDisabled(true);
    d_but_next->setEnabled(true);

    qDebug()<<Q_FUNC_INFO << "end";
}

void ImageButtons::setHotKeys(QString prevPB,
                                   QString nextPB)
{
    qDebug()<<Q_FUNC_INFO;
    d_but_prev->setShortcut(   prevPB);
    d_but_next->setShortcut(  nextPB);

    d_but_prev->setToolTip(    prevPB);
    d_but_next->setToolTip(   nextPB);
}

void ImageButtons::resize(int width, int height)
{
    qDebug()<<Q_FUNC_INFO;
    if(d_isResized) return;
    qDebug()<< "width" <<width;
    qDebug()<< "height" <<height;

    //const uint x = width/2-5 - longSide/2;
    const uint y = height/2.-5 - shortSide/2;

    d_but_prev->setGeometry(0+shortSide/2., y, shortSide, longSide);
    d_but_next->setGeometry(width-1.5*shortSide, y, shortSide, longSide);
    d_isResized = true;
}

void ImageButtons::slot_imagePrev()
{
    qDebug()<<Q_FUNC_INFO;
    if (d_imageNum == 0)
        return;
    d_imageNum--;
    if(d_imageNum == 0)
        d_but_prev->setDisabled(true);
    if(d_imageNum < d_imageCount-1)
        d_but_next->setEnabled(true);
    qDebug()<<Q_FUNC_INFO << "end";
    emit showMeasImg_clicked(d_imageNum);
    //emit imagePrev_clicked(d_imageNum);
}
void ImageButtons::slot_imageNext()
{
    qDebug()<<Q_FUNC_INFO;
    if (d_imageNum >= d_imageCount-1)
        return;
    d_imageNum++;
    if(d_imageNum == d_imageCount-1)
        d_but_next->setDisabled(true);
    if(d_imageNum < d_imageCount-1)
        d_but_prev->setEnabled(true);
    qDebug()<<Q_FUNC_INFO << "end";
    emit showMeasImg_clicked(d_imageNum);
    //emit imageNext_clicked(d_imageNum);
}
