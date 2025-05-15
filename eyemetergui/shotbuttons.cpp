#include "shotbuttons.h"
#include <QDebug>
ShotButtons::ShotButtons(QWidget *parent)
    : QWidget{parent}
{
    qDebug()<<Q_FUNC_INFO;

    d_but_shot = new QPushButton(parent);
    //d_but_prev->setStyleSheet("border-width: 5px; border-color: yellow;");
    //d_but_prev->setStyleSheet(" background-color: yellow; border-radius: 10px; width: 60px; height: 60px;");
//                              "border-width: 2px;"
//                             " border-radius: 10px;"
//                              "border-color: beige;");
    //d_but_shot->setIcon(QIcon(":/img/lightbulb-on.svg"));
    //d_but_prev->setIcon(QIcon(":/img/prev.png"));
    d_but_shot->setStyleSheet(//"color: grey;"
                              "border-image: url(:/img/camera.svg) ;"
                              "background: none;"
                              //"border-image: url(:/img/angle-circle-left.svg) 0 0 0 0;"
                              //"border-radius: 10px;"
                              "border-top: transparent;"
                              "border-bottom: transparent;"
                              "border-right: transparent;"
                              "border-left: transparent;"
                              );
    //d_but_prev->setStyleSheet("border: none");
    d_but_shot->setIconSize(QSize(bigSide, bigSide));
    connect(d_but_shot, SIGNAL(clicked()), SLOT(slot_shot()));

    d_but_back = new QPushButton(parent);
    d_but_back->setStyleSheet(//"background-color: #414447;"
                              //"border-image: url(:/img/next.png) 50 50 50 50;"
                              "border-image: url(:/img/exit.svg) ;"
                              "background: none;"
                              //"border-radius: 10px;"
                              "border-top: transparent;"
                              "border-bottom: transparent;"
                              "border-right: transparent;"
                              "border-left: transparent;"
                              //"border-margin: 20 20 20 20;"
                              );

    d_but_back->setIconSize(QSize(midSide, midSide));
    connect(d_but_back, SIGNAL(clicked()), SLOT(slot_back()));

    d_but_sound = new QPushButton(parent);
    d_but_sound->setStyleSheet(//"background-color: #414447;"
                              //"border-image: url(:/img/next.png) 50 50 50 50;"
                              "border-image: url(:/img/sound.svg) ;"
                               "background: none;"
                              //"border-radius: 10px;"
                              "border-top: transparent;"
                              "border-bottom: transparent;"
                              "border-right: transparent;"
                              "border-left: transparent;"
                              //"border-margin: 20 20 20 20;"
                              );

    d_but_sound->setIconSize(QSize(midSide, midSide));
    connect(d_but_sound, SIGNAL(clicked()), SLOT(slot_sound()));

    d_but_light = new QPushButton(parent);
//    d_but_light->setStyleSheet(//"background-color: #414447;"
//                              //"border-image: url(:/img/next.png) 50 50 50 50;"
//                              "border-image: url(:/img/lightbulb-off.svg) 5 5 5 5;"
//                              "border-radius: 10px;"
//                              "border-top: transparent;"
//                              "border-bottom: transparent;"
//                              "border-right: transparent;"
//                              "border-left: transparent;"
//                              //"border-margin: 20 20 20 20;"
//                              );
    setLightOn(false);

    d_but_light->setIconSize(QSize(midSide, midSide));
    connect(d_but_light, SIGNAL(clicked()), SLOT(slot_light()));
    d_but_light->setDisabled(true);
    //resize(parent->width(), parent->height());
//    d_but_prev->setStyleSheet(" color: white;"
//                  "background-color: #27a9e3;"
//                  "border-width: 0px;"
//                 " border-radius: 30px;  ");
}

void ShotButtons::resize(int width, int height)
{
    qDebug()<<Q_FUNC_INFO;
    if(d_isResized) return;
    qDebug()<< "width" <<width;
    qDebug()<< "height" <<height;

    //const uint x = width/2-5 - longSide/2;
    const uint y = height/2.;

    d_but_back->setGeometry(0+offset, y, midSide, midSide);
    d_but_shot->setGeometry(width-offset - bigSide, y, bigSide, bigSide);
    d_but_sound->setGeometry(0+offset, y/3., midSide, midSide);
    d_but_light->setGeometry(width-offset - midSide, y/3., midSide, midSide);
    d_isResized = true;
}

void ShotButtons::setLightOn(bool on)
{
    qDebug()<<Q_FUNC_INFO;
    d_isLightOn = on;
    if(d_isLightOn)
    {
        d_but_light->setStyleSheet(//"background-color: #414447;"
                                  "border-image: url(:/img/lightbulb-on.svg) 0 0 0 0;"
                                   "background: none;"
                                 // "border-radius: 10px;"
                                  "border-top: transparent;"
                                  "border-bottom: transparent;"
                                  "border-right: transparent;"
                                  "border-left: transparent;"
                                  //"border-margin: 20 20 20 20;"
                                  );
    }
    else
    {
        d_but_light->setStyleSheet("QPushButton { "
                                //"background-color: #414447;"
                                  "border-image: url(:/img/lightbulb-off.svg) ;"
                                   "background: none;"
                                 // "border-radius: 10px;"
                                  "border-top: transparent;"
                                  "border-bottom: transparent;"
                                  "border-right: transparent;"
                                  "border-left: transparent;"
                                  "}"
                                  //"border-margin: 20 20 20 20;"
                                   "QPushButton::disabled { "
                                   "border-image: url(:/img/lightbulb-off_disabled.svg) ;"
                                   "}"
                                  );
    }
}

void ShotButtons::slot_shot()
{
    qDebug()<<Q_FUNC_INFO;
    emit shotBut_clicked(SHOT);
}

void ShotButtons::slot_back()
{
    qDebug()<<Q_FUNC_INFO;
    emit shotBut_clicked(BACK);
}

void ShotButtons::slot_sound()
{
    qDebug()<<Q_FUNC_INFO;
    emit shotBut_clicked(SOUND);
}

void ShotButtons::slot_light()
{
    qDebug()<<Q_FUNC_INFO;
    if (d_isLightOn)
    {
        //d_isLightOn = false;
        emit shotBut_clicked(LIGHTOFF);
    }
    else
    {
        //d_isLightOn = true;
        emit shotBut_clicked(LIGHTON);
    }

}
