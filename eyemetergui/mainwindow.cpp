#include "mainwindow.h"
#include <QToolBar>
#include <QDebug>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << Q_FUNC_INFO;
    createToolBar();
    d_topToolbar = new QToolBar;
    addToolBar(Qt::TopToolBarArea,d_topToolbar);
//d_but_close.setText("Закрыть");
    //
    QIcon closeicon = QIcon::fromTheme("window-close");
    d_but_close.setIcon(closeicon);
    d_but_close.setIconSize(QSize(50,50));
    QWidget* dummy = new QWidget();
    dummy->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    d_topToolbar->addWidget(dummy);
    tb_layout.addWidget(&d_but_close);
    connect(&d_but_close, SIGNAL(clicked()), SLOT(close()));
    //d_topToolbar->setLayout(&tb_layout);
    //setIconSize(QSize(100,100));
    d_topToolbar->addSeparator();
    d_topToolbar->addWidget(&d_but_close);
    //showFullScreen();
    //resize(1500,1500);
    showMaximized();
    //setCentralWidget( &d_l_snapshot);

    d_l_snapshot.setStyleSheet("background-color: black");
    d_measReviewButs = new ImageButtons(&d_l_snapshot);
    //d_measReviewButs->setVisible(false);
    d_measReviewButs->hide(true);
    //d_measReviewButs->hide(false);
    connect(d_measReviewButs, SIGNAL(showMeasImg_clicked(uint)), SLOT(slot_showMeasImg(uint)));
    d_mainWidget = new QWidget();
    //QVBoxLayout mainWinLayout;
    QSizePolicy sp;
    sp.setHeightForWidth(true);
    //d_l_snapshot.setSizePolicy(sp);
    mainWinLayout.addWidget(&d_l_snapshot);
    d_l_snapshot.setAlignment(Qt::AlignCenter);
    mainWinLayout.addWidget(&d_l_measRes);
    d_l_measRes.setFixedHeight(200);
    d_l_measRes.setWordWrap(true);
    QString measRes_str = QString("%1:\n%2:")
                    .arg(CONST_REFRACTION_STR).arg(CONST_INTEROCULAR_STR);
//    AIEyeMeasResult measResult;
//    measResult.left.sphere = 1.3;
//    measResult.left.cylinder = 2.3;
//    measResult.left.angle = 3.3;
//    measResult.left.diameter = 4.3;
//    measResult.right.sphere = 5.3;
//    measResult.right.cylinder = 5.3;
//    measResult.right.angle = 6.3;
//    measResult.right.diameter = 7.3;
//    QString measRes_str = QString("%1:   %2   %3   %4º   %5        %6   %7   %8º   %9\n%10:   %11")
//            .arg(CONST_REFRACTION_STR)     //1
//            .arg(measResult.left.sphere)   //2
//            .arg(measResult.left.cylinder) //3
//            .arg(measResult.left.angle)    //4
//            .arg(measResult.left.diameter) //5
//            .arg(measResult.right.sphere)   //6
//            .arg(measResult.right.cylinder) //7
//            .arg(measResult.right.angle)    //8
//            .arg(measResult.right.diameter) //9
//            .arg(CONST_INTEROCULAR_STR)     //10
//            .arg(measResult.interocular);
    d_l_measRes.setStyleSheet("font: 20px "
                  /*"color: black;"*/);
    d_l_measRes.setText(measRes_str);
    //centralWidgetLayout->addLayout(mainWinLayout);
    d_mainWidget->setLayout(&mainWinLayout);
    setCentralWidget( d_mainWidget);
    //qDebug() << d_l_snapshot.width();
    //qDebug() << d_l_snapshot.height();
    //qDebug() << static_cast<double>(d_l_snapshot.width())/d_l_snapshot.height();
    //qDebug() << d_l_snapshot.width()/CONST_RATIO_WIDTH2HEIGHT;
    //d_l_snapshot.setFixedHeight(d_l_snapshot.width()/CONST_RATIO_WIDTH2HEIGHT);

    initNetwork();

    //connect(&d_measReviewButs, SIGNAL(imagePrev_clicked(uint)), SLOT(slot_imagePrev(uint)));
    //connect(&d_measReviewButs, SIGNAL(imageNext_clicked(uint)), SLOT(slot_imageNext(uint)));
//#ifdef TEST_snapshot
//    d_snapshotParams.frame_height = 1216;
//    d_snapshotParams.frame_width = 1936;
//    d_snapshotParams.size = d_snapshotParams.frame_height * d_snapshotParams.frame_width;
//    d_snapshotParams.buf.resize(d_snapshotParams.size);
//    //uchar buf[d_snapshotParams.size];
//    qDebug() << Q_FUNC_INFO << "file_0";
//    FILE *f; // переменная для работы с файлом

//       f=fopen("/home/moonlight/Main/Work/eyemeter/repa/EyeMeter/eyemetergui/frame1.bin", "r"); // открываем бинарный файл для записи и чтения в режиме добавления, то есть, если файла нет, то он создастся, а если файл есть, то содержимое файла не будет уничтожено, из файла можно будет читать и в файл можно будет записывать

//       qDebug() << Q_FUNC_INFO << f;
//       qDebug() << fread(d_snapshotParams.buf.data() , sizeof(uchar), d_snapshotParams.size, f); // считываем из файла f ровно 1 пакет pack размера int_double

//       fclose(f);
//       qDebug() << Q_FUNC_INFO << "file_end";
//       //memcpy(d_snapshotParams.buf, &buf, d_snapshotParams.size);
//       QPixmap pix = snapshot(d_snapshotParams);
//       d_l_snapshot.setPixmap(pix);
//       //d_l_snapshot.adjustSize();
//#endif
}

MainWindow::~MainWindow()
{
    qDebug() << Q_FUNC_INFO;
    if(d_file_measure.isOpen())
    {
        d_file_measure.flush();
        d_file_measure.close();
    }
    d_udsUniSocket->stop();
}

void MainWindow::createToolBar()
{
    qDebug() << Q_FUNC_INFO;
    //setStyleSheet("QPushButton { color: red; spacing: 20px;  } ");
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
    if(d_measReviewButs != nullptr)
        d_measReviewButs->hide(true);
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
       QPixmap pix = snapshot(d_snapshotParams);
       d_l_snapshot.setPixmap(pix);
       //d_l_snapshot.adjustSize();
#endif
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
//    QString file_measure_str = d_toolbar->name().append('_').append(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss")).append(".bin");
//    qDebug() << "file_measure: " << file_measure_str;
//    d_file_measure.setFileName(file_measure_str);
    d_vec_snapshots.clear();
    d_vec_snapshots.reserve(CONST_MEASURE_SHOTS_COUNT);
    if(d_measReviewButs != nullptr)
        d_measReviewButs->hide(true);
    d_isMeasurStarted = false;
}

void MainWindow::slot_showMeasImg(uint num)
{
    qDebug() << Q_FUNC_INFO;
    if(d_vec_snapshots.size()>static_cast<int>(num))
    {
        QPixmap pix = snapshot( d_vec_snapshots.at(num));
        d_l_snapshot.setPixmap(pix);
        //d_l_snapshot.adjustSize();
    }
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
                QPixmap pix = snapshot(d_snapshotParams);
                qDebug() << "UDSUNI_TITLE_FRAME_READY 3";
                d_l_snapshot.setPixmap(pix);
                if(d_isMeasurStarted)
                    d_vec_snapshots.push_back(d_snapshotParams);
//                if(d_file_measure.isOpen())
//                {
//                    d_file_measure.write(d_snapshotParams.buf.data(),d_snapshotParams.size);
//                    d_file_measure.flush();
//                }
                d_udsUniSocket->send(UDSUNI_TITLE_FRAME_FREE, frame.id, EYEMETER_ROLE_CAM);

            }
        }
        break;
    case UDSUNI_TITLE_MEAS_RUNNING:
        qDebug() << "UDSUNI_TITLE_MEAS_RUNNING";
        if(pack.msg.type == UDSUNI_TYPE_MEASURE_SETTINGS)
        {
                d_isMeasurStarted = true;
                MeasureSettings settings;
                pack.fetch_data(settings);
                //d_shmemBlockReader.reset();
                //d_shmemBlockReader = std::make_unique<ShmemBlockReader>(settings.stream.frame_size, settings.stream.frame_queue_depth, FRAME_SHBUF_NAME);
                d_snapshotParams.frame_height = settings.stream.frame_height;
                d_snapshotParams.frame_width = settings.stream.frame_width;
                d_snapshotParams.size = settings.stream.frame_size;
                d_snapshotParams.buf.resize(d_snapshotParams.size);
//                if(!d_file_measure.isOpen())
//                    d_file_measure.open(QIODevice::WriteOnly | QIODevice::Append);
//                if(d_file_measure.isOpen())
//                {
//                    ;
//                }
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
    {
        qDebug() << "UDSUNI_TITLE_MEAS_SHOOT_DONE";
        d_isMeasurStarted = false;
        QString file_measure_str = d_toolbar->name().append('_').append(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss")).append(".bin");
        qDebug() << "file_measure: " << file_measure_str;
        d_file_measure.setFileName(file_measure_str);
        if(d_file_measure.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            foreach (Snapshot_params shot, d_vec_snapshots) {
                d_file_measure.write(shot.buf.data(),shot.size);
            }
        }        
        d_file_measure.flush();
        d_file_measure.close();
        /*int ret = */QMessageBox::information(this, tr("Измерения завершены."),
                                               tr("СНЯТО."),
                                               QMessageBox::Ok,
                                               QMessageBox::Ok);
        if(d_measReviewButs != nullptr)
        {
            d_measReviewButs->hide(false);
            d_measReviewButs->setImageCount(d_vec_snapshots.size());
        }
        d_vec_snapshots.clear();
        qDebug() << "UDSUNI_TITLE_MEAS_SHOOT_DONE end";
    }
        break;
    case UDSUNI_TITLE_MEAS_FAILED:
        qDebug() << "UDSUNI_TITLE_MEAS_FAILED";
        d_isMeasurStarted = false;
        d_file_measure.close();
        /*int ret = */QMessageBox::information(this, tr("Измерения завершены."),
                                               tr("Ошибка."),
                                               QMessageBox::Ok,
                                               QMessageBox::Ok);
        break;
    case UDSUNI_TITLE_MEAS_RESULT:
    {
        qDebug() << "UDSUNI_TITLE_MEAS_RESULT";
        AIEyeMeasResult measResult;
        pack.fetch_data(measResult);
        QString measRes_str = QString("%1:   %2   %3   %4º   %5        %6   %7   %8º   %9\n%10:   %11")
                .arg(CONST_REFRACTION_STR)     //1
                .arg(measResult.left.sphere)   //2
                .arg(measResult.left.cylinder) //3
                .arg(measResult.left.angle)    //4
                .arg(measResult.left.diameter) //5
                .arg(measResult.right.sphere)   //6
                .arg(measResult.right.cylinder) //7
                .arg(measResult.right.angle)    //8
                .arg(measResult.right.diameter) //9
                .arg(CONST_INTEROCULAR_STR)     //10
                .arg(measResult.interocular);
//        d_l_measRes.setStyleSheet("font: 20px "
//                      /*"color: black;"*/);
        d_l_measRes.setText(measRes_str);
    }
        break;
    default:
        break;
    }
    pack.clear_data();


    qDebug()<<Q_FUNC_INFO << "end";
}

QPixmap MainWindow::snapshot(const Snapshot_params &snapshotParams)
{
    qDebug() << Q_FUNC_INFO;
    int bytesPerLine = snapshotParams.frame_width;
    QImage snapshot_img((uchar*)snapshotParams.buf.c_str(), snapshotParams.frame_width, snapshotParams.frame_height, bytesPerLine, QImage::Format_Grayscale8 /*QImage::Format_Indexed8, QImageCleanupFunction cleanupFunction = nullptr, void *cleanupInfo = nullptr*/);
    return QPixmap::fromImage(snapshot_img.scaled(d_l_snapshot.size(), Qt::KeepAspectRatio, Qt::FastTransformation));
}

void MainWindow::setPhotoScreen()
{

}

void MainWindow::resizeEvent(QResizeEvent* /*event*/)
{
    qDebug() << Q_FUNC_INFO;
    if(d_measReviewButs != nullptr)
        d_measReviewButs->resize(d_l_snapshot.width(),d_l_snapshot.height());
//    if(d_lastShotWidth != d_l_snapshot.width())
//    {
//        qDebug() << "d_lastShotWidth" << d_lastShotWidth;
//        qDebug() << "d_l_snapshot.width()" << d_l_snapshot.width();
//        qDebug() << "d_l_snapshot.height()" << d_l_snapshot.height();
//        qDebug() << "w/h" << static_cast<double>(d_l_snapshot.width())/d_l_snapshot.height();
//        qDebug() << "h" << d_l_snapshot.width()/CONST_RATIO_WIDTH2HEIGHT;
//        d_l_snapshot.setFixedHeight(d_l_snapshot.width()/CONST_RATIO_WIDTH2HEIGHT);
//        d_lastShotWidth = d_l_snapshot.width();
//    }
}
