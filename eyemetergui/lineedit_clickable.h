#ifndef LINEEDIT_CLICKABLE_H
#define LINEEDIT_CLICKABLE_H

//#include <QLineEdit>
#include <QPlainTextEdit>
#include <QMouseEvent>

class LineEdit_Clickable : public QPlainTextEdit//QLineEdit
{
    Q_OBJECT
public:
    //LineEdit_Clickable();
    explicit LineEdit_Clickable(QWidget *parent = nullptr);
    protected:
        //Override mousePressEvent event
        virtual void mousePressEvent(QMouseEvent *event);

    signals:
        //Custom clicked() signal, triggered when the mousePressEvent event occurs
        void clicked();

};

#endif // LINEEDIT_CLICKABLE_H
