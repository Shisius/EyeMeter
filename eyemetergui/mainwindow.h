#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include "emtoolbar.h"
#include "udsunisocket.h"
#include "shmem_alloc.h"
//#include "lineedit_clickable.h"
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
    QLabel d_l_snapshot;
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
    void slot_readUds(const UdsUniPack &pack);
    QPixmap snapshot();
};
#endif // MAINWINDOW_H
