#ifndef DATEEDIT_KEYBOARD_H
#define DATEEDIT_KEYBOARD_H

#include <QDateEdit>

class DateEdit_Keyboard : public QDateEdit
{
    Q_OBJECT
public:
    DateEdit_Keyboard(QWidget *parent = nullptr);
    DateEdit_Keyboard(const QDate &date, QWidget *parent = nullptr);
protected:
    bool event(QEvent*) override;

private:
    bool _moved = false;
    int _lastDiff = 0;
};

#endif // DATEEDIT_KEYBOARD_H
