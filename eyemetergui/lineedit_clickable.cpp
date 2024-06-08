#include "lineedit_clickable.h"
#include <QDebug>
//LineEdit_Clickable::LineEdit_Clickable()
//{

//}
LineEdit_Clickable::LineEdit_Clickable(QWidget *parent) :
    //QLineEdit(parent)
    QPlainTextEdit(parent)
{

}

//Rewrite the mousePressEvent event to detect whether the event type has clicked the left mouse button
void LineEdit_Clickable::mousePressEvent(QMouseEvent *event)
{
   qDebug() << Q_FUNC_INFO;
    //If clicked, it will trigger the clicked signal
    if (event->button() == Qt::LeftButton)
    {
        //Trigger clicked signal
        emit clicked();
    }
    //Pass the event to the parent class for processing
   //QLineEdit::mousePressEvent(event);
    QPlainTextEdit::mousePressEvent(event);
}
