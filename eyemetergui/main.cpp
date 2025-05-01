#include "mainwindow.h"
//#include <QMainWindow>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFile>
#include <QScopedPointer>
#include <QTextStream>
#ifdef DEBUG2FILE
QScopedPointer<QFile>   m_logFile;
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
#endif
int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    qputenv("QT_VIRTUALKEYBOARD_STYLE", "retro");
    //qputenv("QT_VIRTUALKEYBOARD_LAYOUT_PATH", ":/Style/test");
    qRegisterMetaType<UdsUniPack>("UdsUniPack");
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
#ifdef DEBUG2FILE
    QDir curdir = QDir::current();
    QString path("logs");
    curdir.mkdir(path);
//    QFile file("settings.conf");
//    QString path;
//    if(file.open(QIODevice::ReadOnly)){
//        QTextStream in(&file);

//        in >> path;

//        qDebug()<<path;
//        file.close();
//       }

    m_logFile.reset(new QFile(QString("%1/log_%2.txt").arg(path).arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss"))));
    //m_logFile.reset(new QFile(QString("logs/log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss"))));
//    // Открываем файл логирования
    m_logFile.data()->open(QFile::Append | QFile::Text);
    m_logFile.data()->write("test text");
    // Устанавливаем обработчик
    qInstallMessageHandler(messageHandler);
#endif
    //a.setStyleSheet("QApplication::close-button { image: url(:/img/next.png);icon-size: 14px;}");

    MainWindow w;
//    QMainWindow window(nullptr, Qt::FramelessWindowHint);
//    LineEdit_Keyboard lineedit1(&window);
//        lineedit1.move(100, 450);

//        LineEdit_Keyboard lineedit2(&window);
//        lineedit2.move(100, 100);
//        window.resize(1024, 768);
//            window.show();
#ifdef TEST_snapshot
    w.setFixedSize(screen_w,screen_h);
    w.show();
#elif
    w.showFullScreen();
#endif
    return a.exec();
}
#ifdef DEBUG2FILE
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Открываем поток записи в файл
    QTextStream out(m_logFile.data());
    // Записываем дату записи
    //out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    // По типу определяем, к какому уровню относится сообщение
//    switch (type)
//    {
//    case QtInfoMsg:     out << "INF "; break;
//    case QtDebugMsg:    out << "DBG "; break;
//    case QtWarningMsg:  out << "WRN "; break;
//    case QtCriticalMsg: out << "CRT "; break;
//    case QtFatalMsg:    out << "FTL "; break;
//    }
    // Записываем в вывод категорию сообщения и само сообщение
    //out << context.category << ": "
    out    << msg << "\n";/*<< Qt::endl*/;
    out.flush();    // Очищаем буферизированные данные
}
#endif
