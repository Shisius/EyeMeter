#include "inputnamedialog.h"
//#include <QLineEdit>
#include <QKeyEvent>
#include <QDebug>
InputNameDialog::InputNameDialog(QWidget *parent)
    :QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    qDebug() << Q_FUNC_INFO;
    d_name = new LineEdit_Keyboard(this);
    d_name->setBaseSize(size());
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowModality(Qt::WindowModal);
    //QPushButton*
}

InputNameDialog::InputNameDialog(QSize size, QString str, QWidget* parent)
    :QDialog(parent/*, Qt::WindowTitleHint | Qt::WindowSystemMenuHint*/)
{
    qDebug() << Q_FUNC_INFO;
    d_name = new LineEdit_Keyboard(str,this);
    setMinimumSize(size);
    d_name->setMinimumSize(size);
    setWindowFlag(Qt::FramelessWindowHint);
    //setWindowModality(/*Qt::WindowModal*/Qt::ApplicationModal);
    setWindowModality(Qt::WindowModal);
    setStyleSheet("font: bold 50px");
    d_name->setAlignment(Qt::AlignHCenter);
}

void InputNameDialog::keyPressEvent(QKeyEvent *evt)
{
    qDebug() << Q_FUNC_INFO;
    if(evt->key() == Qt::Key_Enter || evt->key() == Qt::Key_Return)
    {
        qDebug() << Q_FUNC_INFO << "Qt::Key_Enter";
        d_str_name = d_name->text();
        accept();
        close();
        return;
    }
    QDialog::keyPressEvent(evt);
}

QString InputNameDialog::name() const
{
    qDebug() << Q_FUNC_INFO;
    return d_str_name;
}
