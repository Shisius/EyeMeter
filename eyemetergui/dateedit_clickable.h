#ifndef DATEEDIT_CLICKABLE_H
#define DATEEDIT_CLICKABLE_H

#include <QDateEdit>

class DateEdit_Clickable : public QDateEdit
{
    Q_OBJECT
public:
    DateEdit_Clickable(QWidget *parent = nullptr);
protected:
    //Override mousePressEvent event
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    //Custom clicked() signal, triggered when the mousePressEvent event occurs
    void clicked();
};

#endif // DATEEDIT_CLICKABLE_H
