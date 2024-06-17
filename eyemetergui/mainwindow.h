#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QFile>
#include "emtoolbar.h"
#include "udsunisocket.h"
#include "shmem_alloc.h"
#include "imagebuttons.h"
#include <QLayout>
//#include "lineedit_keyboard.h"
//#include "inputnamedialog.h"
//#include <QLineEdit>
//#include <QtVirtualKeyboard>


class MainWindow : public QMainWindow
{
    Q_OBJECT

    QMenu *d_menu;
    EMToolBar *d_toolbar = nullptr;
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
    QLabel d_l_snapshot;
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
    void createToolBar();
    void setPhotoScreen();
    void initNetwork();
    //QVBoxLayout resultInfoLayout();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
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
};
#endif // MAINWINDOW_H
