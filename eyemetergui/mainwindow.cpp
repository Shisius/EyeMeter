#include "mainwindow.h"
#include <QToolBar>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << Q_FUNC_INFO;
    createToolBar();
    //showFullScreen();
    resize(1500,1500);
    setCentralWidget( &d_l_snapshot);
    d_l_snapshot.setStyleSheet("background-color: black");
    initNetwork();
#ifdef TEST_snapshot
    d_snapshotParams.frame_height = 1216;
    d_snapshotParams.frame_width = 1936;
    d_snapshotParams.size = d_snapshotParams.frame_height * d_snapshotParams.frame_width;
    d_snapshotParams.buf.resize(d_snapshotParams.size);
    //uchar buf[d_snapshotParams.size];
    qDebug() << Q_FUNC_INFO << "file_0";
    FILE *f; // переменная для работы с файлом

       f=fopen("/home/moonlight/Main/Work/eyemeter/repa/EyeMeter/eyemetergui/frame1.bin", "r"); // открываем бинарный файл для записи и чтения в режиме добавления, то есть, если файла нет, то он создастся, а если файл есть, то содержимое файла не будет уничтожено, из файла можно будет читать и в файл можно будет записывать

       qDebug() << Q_FUNC_INFO << f;
       qDebug() << fread(d_snapshotParams.buf.data() , sizeof(uchar), d_snapshotParams.size, f); // считываем из файла f ровно 1 пакет pack размера int_double

       fclose(f);
       qDebug() << Q_FUNC_INFO << "file_end";
       //memcpy(d_snapshotParams.buf, &buf, d_snapshotParams.size);
       QPixmap pix = snapshot();
       d_l_snapshot.setPixmap(pix);
#endif
}

MainWindow::~MainWindow()
{
    qDebug() << Q_FUNC_INFO;
    d_file_measure.flush();
    d_file_measure.close();
    d_udsUniSocket->stop();
}

void MainWindow::createToolBar()
{
    qDebug() << Q_FUNC_INFO;
    d_toolbar = new EMToolBar(this);
    addToolBar(Qt::LeftToolBarArea,d_toolbar);
    qDebug() << connect(d_toolbar, SIGNAL(sig_startTriggered()), SLOT(slot_start()));
    qDebug() << connect(d_toolbar, SIGNAL(sig_pwrTriggered()), SLOT(slot_pwr()));
    qDebug() << connect(d_toolbar, SIGNAL(sig_measureTriggered()), SLOT(slot_measure()));
}

void MainWindow::slot_start()
{
    qDebug() << Q_FUNC_INFO;
    if(d_udsUniSocket == nullptr)
        return;
    d_udsUniSocket->send(UdsUniTitle::UDSUNI_TITLE_STREAM_START);
}

void MainWindow::slot_pwr()
{
    qDebug() << Q_FUNC_INFO;
    if(d_udsUniSocket == nullptr)
        return;
    int pwr = 1;
    d_udsUniSocket->send(UdsUniTitle::UDSUNI_TITLE_LED_PWR, pwr, EYEMETER_ROLE_CAM);
}

void MainWindow::slot_measure()
{
    qDebug() << Q_FUNC_INFO;
    if(d_udsUniSocket == nullptr)
        return;
    d_udsUniSocket->send(UdsUniTitle::UDSUNI_TITLE_MEAS_START);
    QString file_measure_str = d_toolbar->name().append('_').append(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss")).append(".bin");
    qDebug() << "file_measure: " << file_measure_str;
    d_file_measure.setFileName(file_measure_str);
}

void MainWindow::initNetwork()
{
    qDebug() << Q_FUNC_INFO;
    d_udsUniSocket = new UdsUniSocket();
    qDebug() << d_udsUniSocket->isValid();
    if(!d_udsUniSocket->isValid())
        return;
    qDebug() << connect(d_udsUniSocket, SIGNAL(readyRead(UdsUniPack)), SLOT(slot_readUds(UdsUniPack)));
    if(d_toolbar != nullptr)
    {
        d_toolbar->setStartEnabled(true);
        d_toolbar->setPwrEnabled(true);
        d_toolbar->setMeasureEnabled(true);
    }
}

void MainWindow::slot_readUds(UdsUniPack pack)
{
    qDebug() << Q_FUNC_INFO;
    if(pack.msg.proto != UDSUNI_PROTO_PTTS4)
        return;
    switch (pack.msg.title) {
    case UDSUNI_TITLE_STREAM_RUNNING:
        qDebug() << "UDSUNI_TITLE_STREAM_RUNNING";
        if(pack.msg.type == UDSUNI_TYPE_STREAM_SETTINGS)
        {
            d_shmemBlockReader.reset();
            StreamSettings settings;
            pack.fetch_data(settings);
            d_snapshotParams.frame_height = settings.frame_height;
            d_snapshotParams.frame_width = settings.frame_width;
            d_snapshotParams.size = settings.frame_size;
            d_snapshotParams.buf.resize(d_snapshotParams.size);
            d_shmemBlockReader = std::make_unique<ShmemBlockReader>(settings.frame_size, settings.frame_queue_depth, FRAME_SHBUF_NAME);
            int res=d_shmemBlockReader->setup();
            if(res==-1) ;//????????????????
        }
        break;
    case UDSUNI_TITLE_FRAME_READY:
        qDebug() << "UDSUNI_TITLE_FRAME_READY";
        if(pack.msg.type == UDSUNI_TYPE_SHARED_FRAME)
        {
            if(d_shmemBlockReader!=nullptr)
            {
                SharedFrame frame;
                pack.fetch_data(frame);
                qDebug() << "SharedFrame::id " <<frame.id;
                ShmemBlock block;
                block.id = frame.id;
                qDebug() << "UDSUNI_TITLE_FRAME_READY 0";
                if(d_shmemBlockReader->get_block(block) < 0)
                    break;
                qDebug() << "UDSUNI_TITLE_FRAME_READY 1";
                if(block.ptr == nullptr || block.size == 0)
                    break;
                memcpy(d_snapshotParams.buf.data(), block.ptr, block.size);
                qDebug() << "UDSUNI_TITLE_FRAME_READY 2";
                QPixmap pix = snapshot();
                qDebug() << "UDSUNI_TITLE_FRAME_READY 3";
                d_l_snapshot.setPixmap(pix);
                if(d_file_measure.isOpen())
                {
                    d_file_measure.write(d_snapshotParams.buf.data(),d_snapshotParams.size);
                    d_file_measure.flush();
                }
                d_udsUniSocket->send(UDSUNI_TITLE_FRAME_FREE, frame.id, EYEMETER_ROLE_CAM);

            }
        }
        break;
    case UDSUNI_TITLE_MEAS_RUNNING:
        qDebug() << "UDSUNI_TITLE_MEAS_RUNNING";
        if(pack.msg.type == UDSUNI_TYPE_MEASURE_SETTINGS)
        {

                MeasureSettings settings;
                pack.fetch_data(settings);
                //d_shmemBlockReader.reset();
                //d_shmemBlockReader = std::make_unique<ShmemBlockReader>(settings.stream.frame_size, settings.stream.frame_queue_depth, FRAME_SHBUF_NAME);
                d_snapshotParams.frame_height = settings.stream.frame_height;
                d_snapshotParams.frame_width = settings.stream.frame_width;
                d_snapshotParams.size = settings.stream.frame_size;
                d_snapshotParams.buf.resize(d_snapshotParams.size);
                if(!d_file_measure.isOpen())
                    d_file_measure.open(QIODevice::WriteOnly | QIODevice::Append);
                if(d_file_measure.isOpen())
                {
                    ;
                }
//                qDebug() << "SharedFrame::id " <<frame.id;
//                ShmemBlock block;
//                block.id = frame.id;
//                d_shmemBlockReader->get_block(block);
//                memcpy(d_snapshotParams.buf.data(), block.ptr, block.size);
//                QPixmap pix = snapshot();
//                d_l_snapshot.setPixmap(pix);
//                d_udsUniSocket->send(UDSUNI_TITLE_FRAME_FREE, frame.id, EYEMETER_ROLE_CAM);
            //}
        }
        break;
    case UDSUNI_TITLE_MEAS_SHOOT_DONE:
        qDebug() << "UDSUNI_TITLE_MEAS_SHOOT_DONE";
        d_file_measure.flush();
        d_file_measure.close();
        break;
    default:
        break;
    }
    pack.clear_data();



}

QPixmap MainWindow::snapshot()
{
    qDebug() << Q_FUNC_INFO;
    int bytesPerLine = d_snapshotParams.frame_width;
    QImage snapshot_img((uchar*)d_snapshotParams.buf.c_str(), d_snapshotParams.frame_width, d_snapshotParams.frame_height, bytesPerLine, QImage::Format_Grayscale8 /*QImage::Format_Indexed8, QImageCleanupFunction cleanupFunction = nullptr, void *cleanupInfo = nullptr*/);
    return QPixmap::fromImage(snapshot_img);
}

void MainWindow::setPhotoScreen()
{

}

