#include "mainwindow.h"
//#include <QMainWindow>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "lineedit_keyboard.h"
int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "eyemetergui_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
//    QMainWindow window(nullptr, Qt::FramelessWindowHint);
//    LineEdit_Keyboard lineedit1(&window);
//        lineedit1.move(100, 450);

//        LineEdit_Keyboard lineedit2(&window);
//        lineedit2.move(100, 100);
//        window.resize(1024, 768);
//            window.show();
    w.show();
    return a.exec();
}
