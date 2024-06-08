#include "inputdatedialog.h"
#include <QKeyEvent>
#include <QDebug>
#include <QComboBox>
InputDateDialog::InputDateDialog(QWidget *parent)
    :QDialog(parent/*, Qt::WindowTitleHint | Qt::WindowSystemMenuHint*/)
{
    qDebug() << Q_FUNC_INFO;
    d_de_date = new DateEdit_Keyboard(this);
    d_de_date->setBaseSize(size());
    //d_de_date->setStyleSheet("color: blue; background-color: yellow");
    //setStyleSheet("QDateEdit::drop-down { width:50px; height:15px;background-color: red;}");
    //        image: url(:/new/myapp/cbarrowdn.png);
    //        width:50px;
    //        height:15px;
    //        subcontrol-position: right top;
    //        subcontrol-origin:margin;
    //        background-color: white;
    //        border-style: solid;
    //        border-width: 4px;
    //        border-color: rgb(100,100,100);
    //       spacing: 5px;
    //    }
    //                  ");
//    QDateEdit::UpDownArrows
//    /*QDateEdit::up-button*/ {
//        subcontrol-origin: border;
//        subcontrol-position: top right; /* position at the top right corner */

//        width: 16px; /* 16 + 2*1px border-width = 15px padding + 3px parent border */
//        border-image: url(:/images/spinup.png) 1;
//        border-width: 1px;
//    }

    setWindowFlag(Qt::FramelessWindowHint);
    setWindowModality(Qt::WindowModal);
}

InputDateDialog::InputDateDialog(QSize size, QDate date, QWidget* parent)
    :QDialog(parent/*, Qt::WindowTitleHint | Qt::WindowSystemMenuHint*/)
{
    qDebug() << Q_FUNC_INFO;
    d_de_date = new DateEdit_Keyboard(date, this);

    setMinimumSize(size);
    d_de_date->setMinimumSize(size);
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowModality(Qt::WindowModal);
    setStyleSheet("font: bold 50px");
    d_de_date->setAlignment(Qt::AlignHCenter);
}

void InputDateDialog::keyPressEvent(QKeyEvent *evt)
{
    qDebug() << Q_FUNC_INFO;
    if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
    {
        qDebug() << Q_FUNC_INFO << "Qt::Key_Enter";
        d_date = d_de_date->date();
        accept();
        close();
        return;
    }
    QDialog::keyPressEvent(evt);
}

QDate InputDateDialog::date() const
{
    qDebug() << Q_FUNC_INFO;
    return d_date;
}
