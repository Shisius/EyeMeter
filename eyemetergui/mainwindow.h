#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QFile>
#include "emtoolbar.h"
#include "udsunisocket.h"
#include "shmem_alloc.h"
#include "imagebuttons.h"
//#include "lineedit_keyboard.h"
//#include "inputnamedialog.h"
//#include <QLineEdit>
//#include <QtVirtualKeyboard>


class MainWindow : public QMainWindow
{
    Q_OBJECT

    QMenu *d_menu;
    EMToolBar *d_toolbar = nullptr;
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
    QLabel d_l_snapshot;
    QFile d_file_measure;
    ImageButtons *d_measReviewButs = nullptr;
    //void createActions();
    //void createMenus();
    void createToolBar();
    void setPhotoScreen();
    void initNetwork();
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
