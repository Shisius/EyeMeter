#include "emtoolbar.h"
#include <QtWidgets>
#include <QDebug>
EMToolBar::EMToolBar(QWidget *parent)
    :QToolBar(parent)
{
    qDebug() << Q_FUNC_INFO;
//    setStyleSheet(" background-color:"
//    "qlineargradient(spread:reflect, x1:0.5, y1:0, x2:0, y2:0,"
//    "stop:0 rgba(91, 204, 233, 100), stop:1 rgba(32, 80, 96,"
//    "100)); ");
    setFixedWidth(210);
    setStyleSheet("background-color: #a9c9c2;"
                  "spacing: 20px; "
                  "font: bold 30px "
                  /*"color: black;"*/);
    //setStyleSheet("QPushButton { color: red; } ");/* spacing between items in the tool bar */
//    d_act_start  = addAction(tr("Начать сеанс"));
    d_but_start.setText(tr("Начать сеанс"));
    d_but_start.setFixedHeight(100);
//    but_start.setStyleSheet("color: #051f34;");
    d_act_start  = addWidget(&d_but_start);
    d_act_start->setDisabled(true);
    qDebug() << connect(d_act_start, SIGNAL(triggered()), SLOT(slot_start()));
    d_but_name.setText(tr("ФИО"));
    d_but_name.setFixedHeight(100);
    //d_le_name = new LineEdit_Clickable();
    //d_le_name->setFixedSize(120, 70);
    //d_act_name = addWidget(d_le_name);
    d_act_name = addWidget(&d_but_name);
    //d_le_name->setPlaceholderText(tr("Введите ФИО пациента"));
    //qDebug() << connect(d_le_name, SIGNAL(clicked()), SLOT(slot_enterName()));
    qDebug() << connect(&d_but_name, SIGNAL(clicked()), SLOT(slot_enterName()));
    //setWindowModality(Qt::WindowModal);
    //d_le_date = new LineEdit_Clickable(/*QDate::currentDate()*/);
    //d_le_date->setFixedSize(120, 30);
    //d_le_date->setDate(QDate::currentDate());
    //d_le_date->setPlaceholderText(tr("Дата рождения"));
    //d_act_date = addWidget(d_le_date);
    //d_le_date->setFocus();
    //qDebug() << connect(d_le_date, SIGNAL(clicked()), SLOT(slot_enterDate()));
    //d_act_pwr  = addAction(tr("Освещение"));
    d_but_pwr.setText(tr("Освещение"));
    d_but_pwr.setFixedHeight(100);
    d_act_pwr = addWidget(&d_but_pwr);
    d_act_pwr->setDisabled(true);
    qDebug() << connect(d_act_pwr, SIGNAL(triggered()), SLOT(slot_pwr()));
    //d_act_measure = addAction(tr("Измерение"));
    d_but_measure.setText(tr("Измерение"));
    d_but_measure.setFixedHeight(100);
    d_act_measure = addWidget(&d_but_measure);
    d_act_measure->setDisabled(true);
    qDebug() << connect(d_act_measure, SIGNAL(triggered()), SLOT(slot_measure()));
//    setStyleSheet("background-color: #a9c9c2;"
//                  "spacing: 20px; " /* spacing between items in the tool bar */
//                  "color: black;");
                 // "{QPushButton { color: red; } }");
    //setStyleSheet( "QPushButton { color: red; } ");
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

void EMToolBar::setMeasureEnabled(bool en)
{
    qDebug() << Q_FUNC_INFO;
    d_act_measure->setEnabled(en);
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
        d_dialog_name = new InputNameDialog(size/*, d_le_name->toPlainText()*//*text*/);
        d_dialog_name->move(100,100);
    }
    if(d_dialog_name->exec())
    {
        d_str_name = d_dialog_name->name();
        //d_le_name->setPlainText/*setText*/(d_str_name);
    }
}

//void EMToolBar::slot_enterDate()
//{
//    qDebug() << Q_FUNC_INFO;
//    //int w = QGuiApplication::primaryScreen()->geometry().width()-200;
//    QSize size = QSize(1000,100);
//    if(d_dialog_date == nullptr)
//    {
//        d_dialog_date = new InputDateDialog(size, QDate::fromString(d_le_date->toPlainText(),"dMyyyy"));
//        d_dialog_date->move(100,100);
//    }
//    //setDisabled(true);
//    if(d_dialog_date->exec())
//    {
//        d_date = d_dialog_date->date();
//        d_le_date->setPlainText(d_date.toString("d.M.yyyy"));
//    }
//}

void EMToolBar::slot_start()
{
    qDebug() << Q_FUNC_INFO;
    emit sig_startTriggered();
}

void EMToolBar::slot_pwr()
{
    qDebug() << Q_FUNC_INFO;
    emit sig_pwrTriggered();
}

void EMToolBar::slot_measure()
{
    qDebug() << Q_FUNC_INFO;
    emit sig_measureTriggered();
}
