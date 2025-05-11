#include "lineedit_keyboard.h"
#include <QGuiApplication>
#include <QDebug>
LineEdit_Keyboard::LineEdit_Keyboard(QWidget *parent) :/*QLineEdit QPlainTextEdit*/QTextEdit(parent) {
    qDebug() << Q_FUNC_INFO;
    setAttribute(Qt::WA_InputMethodEnabled, true);
//    d_l_duplicate = new QLabel();
//    d_l_duplicate->setFixedSize(500,50);
//    d_l_duplicate->hide();
    //qDebug() << connect(this, SIGNAL(textEdited(const QString &)), SLOT(slot_textEdited(const QString &)));
    setInputMethodHints(inputMethodHints()/* | Qt::InputMethodHint::ImhDigitsOnly*/);
    //qDebug() << "editingFinished connect" <<connect(()this, SIGNAL(editingFinished()), SLOT(slot_editingFinished()));

}

LineEdit_Keyboard::LineEdit_Keyboard(const QString& txt, QWidget *parent) : /*QLineEdit QPlainTextEdit*/QTextEdit(txt, parent) {
    qDebug() << Q_FUNC_INFO;
    setAttribute(Qt::WA_InputMethodEnabled, true);
    setInputMethodHints(inputMethodHints()/* | Qt::InputMethodHint::ImhDigitsOnly*/);
//    d_l_duplicate = new QLabel();
//    d_l_duplicate->setFixedSize(500,50);
//    d_l_duplicate->hide();
    //qDebug() << connect(this, SIGNAL(textEdited(const QString &)), SLOT(slot_textEdited(const QString &)));
}

bool LineEdit_Keyboard::event(QEvent* e) {
    //qDebug() << Q_FUNC_INFO;
    const auto keyboard_rect = QGuiApplication::inputMethod()->keyboardRectangle();
    const auto keyboard_visible = QGuiApplication::inputMethod()->isVisible();
    const auto global_y = QWidget::mapToGlobal(rect().topLeft()).y() + height();
    const auto k_global_y = keyboard_rect.topLeft().y() - offset;
    const auto diff = k_global_y - global_y;
    const auto need_to_move = diff < 0;


    /* move main widget */
    if (keyboard_visible && !_moved && need_to_move) {
        qDebug() << Q_FUNC_INFO << "move main widget";
        _moved = true;
        _lastDiff = diff;
        const auto g = parentWidget()->frameGeometry();
        parentWidget()->move(g.x(), g.y() - qAbs(_lastDiff));
        qDebug() << "g.x()" << g.x();
        qDebug() << "g.y()" << g.y();
        qDebug() << "qAbs(_lastDiff)" << qAbs(_lastDiff);
        qDebug() << "g.y() - qAbs(_lastDiff)" << g.y() - qAbs(_lastDiff);
        //d_l_duplicate->show();
        qDebug() << Q_FUNC_INFO << "move main widget end";
    }
    /* roll back */
    if (!keyboard_visible && _moved) {
        qDebug() << Q_FUNC_INFO << "roll back";
        _moved = false;
        const auto g = parentWidget()->frameGeometry();
        parentWidget()->move(g.x(), g.y() + qAbs(_lastDiff));
        qDebug() << "g.x()" << g.x();
        qDebug() << "g.y()" << g.y();
        qDebug() << "qAbs(_lastDiff)" << qAbs(_lastDiff);
        qDebug() << "g.y() + qAbs(_lastDiff)" << g.y() + qAbs(_lastDiff);
        //d_le_duplicate->hide();
        qDebug() << Q_FUNC_INFO << "roll back end";
    }
    return /*QLineEdit QPlainTextEdit*/QTextEdit::event(e);
}

void LineEdit_Keyboard::slot_editingFinished()
{
    qDebug() << Q_FUNC_INFO;
    const auto keyboard_rect = QGuiApplication::inputMethod()->keyboardRectangle();
    const auto keyboard_visible = QGuiApplication::inputMethod()->isVisible();
    const auto global_y = QWidget::mapToGlobal(rect().topLeft()).y() + height();
    const auto k_global_y = keyboard_rect.topLeft().y() - offset;
    const auto diff = k_global_y - global_y;
    const auto need_to_move = diff < 0;

    /* roll back */
    if (/*!keyboard_visible &&*/ _moved) {
        qDebug() << Q_FUNC_INFO << "roll back";
        _moved = false;
        const auto g = parentWidget()->frameGeometry();
        parentWidget()->move(g.x(), g.y() + qAbs(_lastDiff));
        qDebug() << "g.x()" << g.x();
        qDebug() << "g.y()" << g.y();
        qDebug() << "qAbs(_lastDiff)" << qAbs(_lastDiff);
        qDebug() << "g.y() + qAbs(_lastDiff)" << g.y() + qAbs(_lastDiff);
        //d_le_duplicate->hide();
        qDebug() << Q_FUNC_INFO << "roll back end";
    }
}
