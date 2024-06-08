#ifndef INPUTDATEDIALOG_H
#define INPUTDATEDIALOG_H

#include <QDialog>
#include "dateedit_keyboard.h"
class InputDateDialog : public QDialog
{
    Q_OBJECT
    DateEdit_Keyboard * d_de_date;
    QDate d_date;
protected:
    //bool eventFilter(QObject *target, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
public:
    InputDateDialog(QWidget* parent = nullptr);
    InputDateDialog(QSize size, QDate date = QDate::currentDate(), QWidget* parent = nullptr);
    QDate date() const;
};

#endif // INPUTDATEDIALOG_H
