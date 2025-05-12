#ifndef IMAGEBUTTONS_H
#define IMAGEBUTTONS_H

#include <QWidget>
#include <QPushButton>
class ImageButtons: public QWidget
{
    Q_OBJECT
    static constexpr uint longSide = 52;
    static constexpr uint shortSide = longSide;
    QPushButton *d_but_prev;
    QPushButton *d_but_next;
    uint d_imageNum;
    uint d_imageCount;
    bool d_isResized=false;
public:
    explicit ImageButtons(QWidget *parent);
    virtual ~ImageButtons(){}

    void hide(bool);
    void resize(int w, int h);
    //void setButtons(uint imageCount);
    void setImageCount(uint imageCount, uint imageNum = 0);
    void setHotKeys(QString prevPB,
                    QString rightPB);
signals:
    //void imagePrev_clicked(uint);
    //void imageNext_clicked(uint);
    void showMeasImg_clicked(uint);
private slots:
    void slot_imagePrev();
    void slot_imageNext();
//private:
//    void setImageCount(uint imageCount);
};

#endif // IMAGEBUTTONS_H
