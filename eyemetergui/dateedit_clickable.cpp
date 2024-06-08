#include "dateedit_clickable.h"
#include <QMouseEvent>
#include <QDebug>
DateEdit_Clickable::DateEdit_Clickable(QWidget *parent)
    :QDateEdit(parent)
{

}

//Rewrite the mousePressEvent event to detect whether the event type has clicked the left mouse button
void DateEdit_Clickable::mousePressEvent(QMouseEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    //If clicked, it will trigger the clicked signal
    if (event->button() == Qt::LeftButton)
    {
        qDebug() << Q_FUNC_INFO << "Qt::LeftButton";
        //Trigger clicked signal
        emit clicked();
    }
    //Pass the event to the parent class for processing
   //QLineEdit::mousePressEvent(event);
    QDateEdit::mousePressEvent(event);
}
