#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QFile>
//#include "emtoolbar.h"
#include "udsunisocket.h"
#include "shmem_alloc.h"
#include "imagebuttons.h"
#include <QLayout>
#include "lineedit_keyboard.h"
//#include "inputnamedialog.h"
//#include <QLineEdit>
//#include <QtVirtualKeyboard>


class MainWindow : public QMainWindow
{
    Q_OBJECT
//#ifdef NEWVISION

    //QScopedPointer<QFile>   d_ptr_logFile;
    //QScopedPointer<QDebug> d_ptr_logDebug;
    //QDebug logDebug(m_logFile.data());
    QTabWidget* d_tab_central;
    enum class tabWidget
    {
        CARD, //patient card
        MEAS //measure screen
    };
    QFrame *d_frame_card;

    QLineEdit *d_le_firstName;
    QLineEdit *d_le_lastName;
    QLineEdit *d_le_middleName;
    QDateEdit *d_de_birthDate;
    QLineEdit *d_le_id;    
    LineEdit_Keyboard* d_pte_disease;

    QString d_str_firstName;
    QString d_str_lastName;
    QString d_str_middleName;
    QDate d_date_birthDate;    
    QString d_str_id;
    QString d_str_disease;

    QPushButton *d_pb_start;

    QLabel *d_l_leftEye;
    QLabel *d_l_rightEye;
    QLabel *d_l_eyes;
    QLabel *d_l_refractionLeft;
    QLabel *d_l_refractionRight;
    QLabel *d_l_diameterLeft;
    QLabel *d_l_diameterRight;
    QLabel *d_l_interocularRes;
    QLabel *d_l_pic_FixLeft;
    QLabel *d_l_pic_FixRight;
    QPicture fixation(int side, QColor grid = Qt::gray, QColor dot = Qt::red);
    QPushButton *d_pb_shot;
    void setStyle2list(const QList<QLabel*> & list, const QString &style = "", Qt::Alignment = Qt::AlignLeft, const QFont &f = QFont());
    void setStyle2list(const QList<QLineEdit*> & list, const QPalette & p, const QString &style = "", Qt::Alignment = Qt::AlignLeft, const QFont &f = QFont());
    void decorateLine(QFrame * line, const QString style);
    //QLabel *d_l_snapshot;

//#else

//#endif
    QLabel d_l_snapshot;
    //QMenu *d_menu;
    //EMToolBar *d_toolbar = nullptr;
    QToolBar *d_topToolbar = nullptr;
    QPushButton d_but_close;
    UdsUniSocket *d_udsUniSocket = nullptr;
    unique_ptr<ShmemBlockReader> d_shmemBlockReader;
    struct Snapshot_params{
        unsigned int size;
        unsigned int frame_width;
        unsigned int frame_height;
        //uchar *buf;
        std::string buf;
    } d_snapshotParams;
    const uint CONST_MEASURE_SHOTS_COUNT = 80;
    QVector<Snapshot_params> d_vec_snapshots;
    //QFrame d_mainFrame;
    QVBoxLayout mainWinLayout;
    QHBoxLayout tb_layout;
    QWidget *d_mainWidget;

    const double CONST_RATIO_WIDTH2HEIGHT = 16./10;
    int d_lastShotWidth = 0;
    QFile d_file_measure;
    ImageButtons *d_measReviewButs = nullptr;
    bool d_isMeasurStarted = false;
    QLabel d_l_measRes;

    const QString CONST_REFRACTION_STR = "Рефракция";
    const QString CONST_INTEROCULAR_STR = "Межзрачковое расстояние";
//    enum class MeasureResults
//    {
//        sphere,
//        cylinder,
//        angle,
//        diameter,
//        interocular
//    };
//    const QMap<MeasureResults, QString> CONST_MEASURE_RESULTS_STRING_MAP = {
//        {MeasureResults::sphere, ""}
//    }
    //void createActions();
    //void createMenus();
    //void createToolBar();
    void setPhotoScreen();
    void initNetwork();
    //QVBoxLayout resultInfoLayout();
    void measFinished(const QString &res);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void slot_diseaseTextChanged();
    void slot_start();
    void slot_pwr();
    void slot_measure();
    //void slot_nextImg(uint);
    //void slot_prevImg(uint);
    void slot_showMeasImg(uint);
    //void slot_readUds(const UdsUniPack &pack);
    void slot_readUds(UdsUniPack pack);
    QPixmap snapshot(const Snapshot_params &);
protected:
    void resizeEvent(QResizeEvent* event) override;
    //bool event(QEvent*) override;
};
#endif // MAINWINDOW_H
