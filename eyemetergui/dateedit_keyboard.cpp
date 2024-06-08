#include "dateedit_keyboard.h"
#include <QGuiApplication>
#include <QDebug>
DateEdit_Keyboard::DateEdit_Keyboard(QWidget *parent)
    :QDateEdit(parent)
{
    setAttribute(Qt::WA_InputMethodEnabled, true);
    //setStyleSheet("color: blue; background-color: yellow");
    setStyleSheet("up-button { subcontrol-origin: border;subcontrol-position: top right; width: 16px; border-width: 1px;}");
}

DateEdit_Keyboard::DateEdit_Keyboard(const QDate &date, QWidget *parent)
    :QDateEdit(date, parent)
{
    setAttribute(Qt::WA_InputMethodEnabled, true);
    //setStyleSheet("color: blue; background-color: yellow");
//    setStyleSheet("QDateEdit::up-button,  QDateEdit::down-button{ "
//                  "subcontrol-origin: border;"
//                  "subcontrol-position: top right; "
//                  "border-width: 1px; "
//                  "border: 2px solid red;"
//                  "width: 30px;"
//                  "height: 30px;"
//                  "background: green;}");
    //setDisplayFormat("d.M.yyyy");
}

bool DateEdit_Keyboard::event(QEvent* e) {
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
    return QDateEdit::event(e);
}
