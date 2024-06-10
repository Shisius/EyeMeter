#ifndef EMTOOLBAR_H
#define EMTOOLBAR_H

#include <QToolBar>
#include <QDateEdit>
#include <QPushButton>
#include "lineedit_clickable.h"
#include "dateedit_clickable.h"
#include "inputnamedialog.h"
#include "inputdatedialog.h"
class EMToolBar : public QToolBar
{
    Q_OBJECT

    QAction *d_act_start;
    QPushButton but_start;
    QAction *d_act_name;
    QAction *d_act_date;
    QAction *d_act_measure;
    QAction *d_act_pwr;
    LineEdit_Clickable *d_le_name;
    InputNameDialog *d_dialog_name = nullptr;
    //DateEdit_Clickable *d_de_date;
    LineEdit_Clickable *d_le_date;
    InputDateDialog *d_dialog_date = nullptr;
    QString d_str_name;
    QDate d_date;
public:
    EMToolBar(QWidget* parent = nullptr);
    QString name() const;
    void setStartEnabled(bool);
    void setPwrEnabled(bool);
    void setMeasureEnabled(bool);

private slots:
    void slot_enterName();
    void slot_enterDate();
    void slot_start();
    void slot_pwr();
    void slot_measure();
signals:
    void sig_startTriggered();
    void sig_pwrTriggered();
    void sig_measureTriggered();
};

#endif // EMTOOLBAR_H
