#ifndef SHOTBUTTONS_H
#define SHOTBUTTONS_H

#include <QWidget>
#include <QPushButton>
enum SHOT_BUTTON
{
    SHOT,
    BACK, //
    SOUND, //
    LIGHTON,//
    LIGHTOFF
};
class ShotButtons : public QWidget
{
    Q_OBJECT
    static constexpr uint bigSide = 52;
    static constexpr uint midSide = 40;
    static constexpr uint offset = 20;

    QPushButton *d_but_shot;
    QPushButton *d_but_back;
    QPushButton *d_but_sound;
    QPushButton *d_but_light;

    bool d_isResized=false;
    bool d_isLightOn = false;
public:
    explicit ShotButtons(QWidget *parent);
    virtual ~ShotButtons(){}
    void resize(int w, int h);
    void setLightOn(bool);
signals:
    void shotBut_clicked(uint);
private slots:
    void slot_shot();
    void slot_back();
    void slot_sound();
    void slot_light();
};

#endif // SHOTBUTTONS_H
