#include "lineedit_keyboard.h"
#include <QGuiApplication>
#include <QDebug>
LineEdit_Keyboard::LineEdit_Keyboard(QWidget *parent) :QLineEdit(parent) {
    qDebug() << Q_FUNC_INFO;
    setAttribute(Qt::WA_InputMethodEnabled, true);
    d_l_duplicate = new QLabel();
    d_l_duplicate->setFixedSize(500,50);
    d_l_duplicate->hide();
    qDebug() << connect(this, SIGNAL(textEdited(const QString &)), SLOT(slot_textEdited(const QString &)));
    //setInputMethodHints(inputMethodHints() | Qt::InputMethodHint::ImhDigitsOnly);

}

LineEdit_Keyboard::LineEdit_Keyboard(const QString& txt, QWidget *parent) : QLineEdit(txt, parent) {
    qDebug() << Q_FUNC_INFO;
    setAttribute(Qt::WA_InputMethodEnabled, true);
    //setInputMethodHints(inputMethodHints() | Qt::InputMethodHint::ImhDigitsOnly);
    d_l_duplicate = new QLabel();
    d_l_duplicate->setFixedSize(500,50);
    d_l_duplicate->hide();
    qDebug() << connect(this, SIGNAL(textEdited(const QString &)), SLOT(slot_textEdited(const QString &)));
}

bool LineEdit_Keyboard::event(QEvent* e) {
    qDebug() << Q_FUNC_INFO;
    const auto keyboard_rect = QGuiApplication::inputMethod()->keyboardRectangle();
    const auto keyboard_visible = QGuiApplication::inputMethod()->isVisible();
    const auto global_y = QWidget::mapToGlobal(rect().topLeft()).y() + height();
    const auto k_global_y = keyboard_rect.topLeft().y();
    const auto diff = k_global_y - global_y;
    const auto need_to_move = diff < 0;


    /* move main widget */
    if (keyboard_visible && !_moved && need_to_move) {
        qDebug() << Q_FUNC_INFO << "move main widget";
        _moved = true;
        _lastDiff = diff;
        const auto g = parentWidget()->frameGeometry();
        parentWidget()->move(g.x(), g.y() - qAbs(_lastDiff));
        //d_l_duplicate->show();
    }
    /* roll back */
    if (!keyboard_visible && _moved) {
        qDebug() << Q_FUNC_INFO << "roll back";
        _moved = false;
        const auto g = parentWidget()->frameGeometry();
        parentWidget()->move(g.x(), g.y() + qAbs(_lastDiff));
        //d_le_duplicate->hide();
    }
    return QLineEdit::event(e);
}

void LineEdit_Keyboard::slot_textEdited(const QString &text)
{
    qDebug() << Q_FUNC_INFO;
    d_l_duplicate->setText(text);
}
