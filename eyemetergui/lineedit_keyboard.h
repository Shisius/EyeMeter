#ifndef LINEEDIT_KEYBOARD_H
#define LINEEDIT_KEYBOARD_H

//#include <QLineEdit>
//#include <QLabel>
#include <QPlainTextEdit>
//#include <QTextEdit>

class LineEdit_Keyboard : public QTextEdit//QPlainTextEdit//QLineEdit
{
    Q_OBJECT

    //QLabel *d_l_duplicate;
public:
    LineEdit_Keyboard(QWidget *parent = nullptr);
    LineEdit_Keyboard(const QString&, QWidget *parent = nullptr);

protected:
    bool event(QEvent*) override;

private:
    bool _moved = false;
    int _lastDiff = 0;
    int offset = 40;
private slots:
    //void slot_textEdited(const QString &text);
};

#endif // LINEEDIT_KEYBOARD_H
