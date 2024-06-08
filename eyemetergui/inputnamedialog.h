#ifndef INPUTNAMEDIALOG_H
#define INPUTNAMEDIALOG_H

#include <QDialog>
#include "lineedit_keyboard.h"
//class QLineEdit;
class InputNameDialog : public QDialog
{
    Q_OBJECT

    //QLineEdit* d_name;
    LineEdit_Keyboard* d_name;
    QString d_str_name;
protected:
    //bool eventFilter(QObject *target, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
public:
    InputNameDialog(QWidget* parent = nullptr);
    InputNameDialog(QSize size, QString str = "", QWidget* parent = nullptr);
    QString name() const;
};

#endif // INPUTNAMEDIALOG_H
