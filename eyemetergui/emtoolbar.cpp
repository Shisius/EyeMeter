#include "emtoolbar.h"
#include <QtWidgets>
#include <QDebug>
EMToolBar::EMToolBar(QWidget *parent)
    :QToolBar(parent)
{
    qDebug() << Q_FUNC_INFO;
    d_act_start  = addAction(tr("Начать сеанс"));
    d_act_start->setDisabled(true);
    qDebug() << connect(d_act_start, SIGNAL(triggered()), SLOT(slot_start()));
    d_le_name = new LineEdit_Clickable();
    d_le_name->setFixedSize(120, 70);
    d_act_name = addWidget(d_le_name);
    d_le_name->setPlaceholderText(tr("Введите ФИО пациента"));
    qDebug() << connect(d_le_name, SIGNAL(clicked()), SLOT(slot_enterName()));
    setWindowModality(Qt::WindowModal);
    d_le_date = new LineEdit_Clickable(/*QDate::currentDate()*/);
    d_le_date->setFixedSize(120, 30);
    //d_le_date->setDate(QDate::currentDate());
    d_le_date->setPlaceholderText(tr("Дата рождения"));
    d_act_date = addWidget(d_le_date);
    //d_le_date->setFocus();
    qDebug() << connect(d_le_date, SIGNAL(clicked()), SLOT(slot_enterDate()));
    d_act_pwr  = addAction(tr("Освещение"));
    d_act_pwr->setDisabled(true);
    qDebug() << connect(d_act_pwr, SIGNAL(triggered()), SLOT(slot_pwr()));
}

QString EMToolBar::name() const
{
    qDebug() << Q_FUNC_INFO;
    return d_str_name;
}

void EMToolBar::setStartEnabled(bool en)
{
    qDebug() << Q_FUNC_INFO;
    d_act_start->setEnabled(en);
}

void EMToolBar::setPwrEnabled(bool en)
{
    qDebug() << Q_FUNC_INFO;
    d_act_pwr->setEnabled(en);
}

void EMToolBar::slot_enterName()
{
    qDebug() << Q_FUNC_INFO;
    int w = QGuiApplication::primaryScreen()->geometry().width()-200;
    QSize size = QSize(w,100);
    if(d_dialog_name == nullptr)
    {
        d_dialog_name = new InputNameDialog(size, d_le_name->toPlainText()/*text*/);
        d_dialog_name->move(100,100);
    }
    if(d_dialog_name->exec())
    {
        d_str_name = d_dialog_name->name();
        d_le_name->setPlainText/*setText*/(d_str_name);
    }
}

void EMToolBar::slot_enterDate()
{
    qDebug() << Q_FUNC_INFO;
    //int w = QGuiApplication::primaryScreen()->geometry().width()-200;
    QSize size = QSize(1000,100);
    if(d_dialog_date == nullptr)
    {
        d_dialog_date = new InputDateDialog(size, QDate::fromString(d_le_date->toPlainText(),"dMyyyy"));
        d_dialog_date->move(100,100);
    }
    //setDisabled(true);
    if(d_dialog_date->exec())
    {
        d_date = d_dialog_date->date();
        d_le_date->setPlainText(d_date.toString("d.M.yyyy"));
    }
}

void EMToolBar::slot_start()
{
    emit sig_startTriggered();
}

void EMToolBar::slot_pwr()
{
    emit sig_pwrTriggered();
}
