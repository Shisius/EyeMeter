#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QFile>
//#include "emtoolbar.h"
#include "udsunisocket.h"
#include "eye_shmem.h"
#include "imagebuttons.h"
#include <QLayout>
#include "lineedit_keyboard.h"
#include "guitextlib.h"
#include "shotbuttons.h"
//#include "styles.h"
//#include <QLineEdit>
//#include <QtVirtualKeyboard>
#ifdef TEST_snapshot
const int screen_w = 1280;//629;
const int screen_h = 800;//401;
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT
    const QString STR_COLD_DARK_COLOR = "#1A5C51";//"#64938d";//"#528c83";
    const QString STR_COLD_DARK_COLOR2 = "#64938d";//"#528c83";
    const QString STR_COLD_LIGHT_COLOR = "#c4e5d4";
    const QString STR_WARM_DARK_COLOR = "#e5a977";
    const QString STR_WARM_LIGHT_COLOR = "#fce6ac";//"#e5a484";//
    const QString STR_MOSTDARK_COLOR = "#3e687e";
    const QString STR_WARM_MOSTDARK_COLOR = "#926C29";//"#B6904E";
    const QString STR_MOSTLIGHT_COLOR = "#fffefe";
    const QString STR_LIGHT_COLOR2 = "#FFF2DB";//"#dddde2";

    const QString STR_TITLE = " "; //"АЙБОЛИТчекс";//"Let you see new horizons";
    const QString STR_error = "error";
    const QString STR_extrametrics = "lsharp, rsharp, lflisk, rflick";
    const QString STR_strabismus = "strabismus";
    const int TOOLBARICON_WIDTH = 60;
    const int TOOLBARICON_HEIGHT = 40;
    const int PB_HEIGHT = 60;

    QString str_color_graphDot = STR_WARM_MOSTDARK_COLOR;

    QStackedWidget *d_stackedWidget_main;
    //QTabWidget* d_tab_central;
    //enum class tabWidget
    enum class stackedWidget_main
    {
        PATIENT, //patient card
        RESULTS,
        ALBUM, //album with shots
        PUPILS,//technological        
        MEAS //measure screen

    };
    QFrame *d_frame_patient;
    QFrame *d_frame_results;
    QFrame *d_frame_pupils;
    QFrame *d_frame_album;
    QFrame *d_frame_meas;
    ShotButtons *d_shotButs = nullptr;
    //QFrame *frame_data_total;
    QFrame *frame_dataResults_total;

    //QLineEdit *d_le_firstName;
    //QLineEdit *d_le_lastName;
    //QLineEdit *d_le_middleName;
    QDateEdit *d_de_birthDate;
    QLineEdit *d_le_id;
    QRadioButton* d_female;
    QRadioButton* d_male;

    LineEdit_Keyboard* d_pte_disease;

    QDate d_date_birthDate;    
    QString d_str_id;
    QString d_str_disease;

    QPushButton *d_pb_newpatient;
    QPushButton *d_pb_start;

    QLabel *d_l_leftEye;
    QLabel *d_l_rightEye;
    QLabel *d_l_eyes;
    bool d_isEyesLabelSizeFixed = false;
    QSize d_eyesLabelSize;
    //QLabel *d_l_refractionLeft;
    //QLabel *d_l_refractionRight;
    QLabel *d_l_sphLeft;
    QLabel *d_l_sphRight;
    QLabel *d_l_cylLeft;
    QLabel *d_l_cylRight;
    QLabel *d_l_aLeft;
    QLabel *d_l_aRight;
    QLabel *d_l_diameterLeft;
    QLabel *d_l_diameterRight;
    QLabel *d_l_interocular;
    QLabel *d_l_strabismus;
    QLabel *d_l_pic_FixLeft;
    QLabel *d_l_pic_FixRight;
    QPicture d_pic_fixGrid;
    QLabel *d_l_error;
    QPushButton *d_pb_reshot;
    QLabel *d_l_extrametrics;

    //QPushButton *d_pb_shot;

    void setStyle2list(const QList<QLabel*> & list, const QString &style = "", Qt::Alignment = Qt::AlignLeft, const QFont &f = QFont());
    void setStyle2list(const QList<QLineEdit*> & list, const QPalette & p, const QString &style = "", Qt::Alignment = Qt::AlignLeft, const QFont &f = QFont());
    void decorateLine(QFrame * line, const QString style);

    QLabel d_l_snapshot;
    QLabel *d_l_photosViewer;

    //FOR TEST

    QSize d_frame_card_lastSize;
    QSize frame_data_total_lastSize;
    QSize frame_dataResults_total_lastSize;
    //QSize d_pb_shot_lastSize ;
    QSize d_l_snapshot_lastSize ;
    QSize d_l_photosViewer_lastSize ;
    QSize d_l_leftEye_lastSize;
    QSize d_l_rightEye_lastSize;
    QSize d_l_eyes_lastSize;
    QSize d_l_refractionLeft_lastSize;
    QSize d_l_refractionRight_lastSize;    
    QSize d_l_interocularRes_lastSize;
    QSize d_l_pic_FixLeft_lastSize;
    QSize d_l_pic_FixRight_lastSize;
    QSize d_measReviewButs_lastSize;

    QFrame *frame_fonts;

    //FOR TEST end

    QToolBar *d_topToolbar = nullptr;
    QButtonGroup *d_grbut_win;
    QPushButton d_but_patient;
    QPushButton d_but_results;
    QPushButton d_but_album;
    QPushButton d_but_pupils;
    QPushButton d_but_close;
    QTimer *d_timer_updateTime;
    QLabel *d_l_currentTime;

    UdsUniSocket *d_udsUniSocket = nullptr;
    unique_ptr<ShmemBlockReader> d_shmemBlockReader;
    unique_ptr<MeasResultShmemReader> d_measResultShmemReader;
    struct Image_params{
        unsigned int size;
        unsigned int frame_width;
        unsigned int frame_height;
        std::string buf;
    } ;
    Image_params d_snapshotParams;
    uint d_measShotsCount;
    QVector<Image_params> d_vec_snapshots;
    AIStreamResult d_last_streamResult;
    bool d_isAIStreamResultGot = false;

    QFile d_file_measure;
    ImageButtons *d_measReviewButs = nullptr;
    bool d_isMeasurStarted = false;
    QLabel d_l_measRes;

    SharedPupilImage d_leftPupil;
    SharedPupilImage d_rightPupil;

    //void createActions();
    //void createMenus();
    //void createToolBar();
    void setPhotoScreen();
    QString savingPath();
    void initNetwork();
    void measFinished(const QString &res);
    QPixmap image(const Image_params &/*, QSize size*/);
    QPixmap ocular_pixmap(const Image_params & params, const EyeCirclePos &left, const EyeCirclePos &right
                          , const QColor &color_left, const QColor &color_right, const QColor &color_line, int lineWidth);
    QPixmap ocular_pix_result(const Image_params & params, const EyeCirclePos &left, const EyeCirclePos &right, QSize size);
    QPixmap ocular_pix_streamFrame(const Image_params & params, const AIStreamResult &streamResult, QSize size);
    QPicture fixation_grid(int side, QColor grid = Qt::gray);
    QPicture fixation_result(const QPicture & grid, std::vector<EyeSkewCoords> skew_vec, QColor dots_color = Qt::red);
    void clearResults();
    void setMeasWin();
    //void initParams();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    //void slot_diseaseTextChanged();

    void slot_idcursorPositionChanged(int oldPos, int newPos);
    //void slot_setPatientWin();
    //void slot_setResultsWin();
    //void slot_setAlbumWin();
    void slot_setPupilsWin();
    void slot_setWin(int id);
//    void slot_idreturnPressed();
//    void slot_idselectionChanged();
//    void slot_idtextChanged(const QString &);
//    void slot_idtextEdited(const QString &);
    void slot_newpatient();
    void slot_start();
    void slot_pwr();
    void slot_measure();
    void slot_showMeasImg(uint);
    void slot_shotButtonClicked(uint);
    //void slot_readUds(const UdsUniPack &pack);
    void slot_readUds(UdsUniPack pack);
    void slot_updateTime();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    //bool event(QEvent*) override;
};
#endif // MAINWINDOW_H
