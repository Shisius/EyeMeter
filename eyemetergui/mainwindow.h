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
//#include "inputnamedialog.h"
//#include <QLineEdit>
//#include <QtVirtualKeyboard>


class MainWindow : public QMainWindow
{
    Q_OBJECT
    const QString STR_COLD_DARK_COLOR = "#528c83";
    const QString STR_COLD_LIGHT_COLOR = "#c4e5d4";
    const QString STR_WARM_DARK_COLOR = "#e5a977";
    const QString STR_WARM_LIGHT_COLOR = "#fce6ac";
    const QString STR_MOSTDARK_COLOR = "#3e687e";
    const QString STR_MOSTLIGHT_COLOR = "#fffefe";
    const QString STR_LIGHT_COLOR2 = "#dddde2";

    const QString STR_TITLE = "ВЕДУТСЯ РЕМОНТНЫЕ РАБОТЫ";//"Let you see new horizons";

    QTabWidget* d_tab_central;
    enum class tabWidget
    {
        HOME,
        CARD, //patient card
        MEAS //measure screen

    };
    QSharedPointer<QFrame> *d_frame_home;
    QFrame *d_frame_card;
    QFrame *frame_data_total;
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

    QPushButton *d_pb_start;

    QLabel *d_l_leftEye;
    QLabel *d_l_rightEye;
    QLabel *d_l_eyes;
    bool d_isEyesLabelSizeFixed = false;
    QSize d_eyesLabelSize;
    QLabel *d_l_refractionLeft;
    QLabel *d_l_refractionRight;    
    QLabel *d_l_interocularRes;
    QLabel *d_l_pic_FixLeft;
    QLabel *d_l_pic_FixRight;
    QPicture d_pic_fixGrid;
    QLabel *d_l_error;
    QLabel *d_l_extrametrics;
    QPushButton *d_pb_shot;
    void setStyle2list(const QList<QLabel*> & list, const QString &style = "", Qt::Alignment = Qt::AlignLeft, const QFont &f = QFont());
    void setStyle2list(const QList<QLineEdit*> & list, const QPalette & p, const QString &style = "", Qt::Alignment = Qt::AlignLeft, const QFont &f = QFont());
    void decorateLine(QFrame * line, const QString style);

    QLabel d_l_snapshot;

    //FOR TEST

    QSize d_frame_card_lastSize;
    QSize frame_data_total_lastSize;
    QSize frame_dataResults_total_lastSize;
    QSize d_pb_shot_lastSize ;
    QSize d_l_snapshot_lastSize ;
    QSize d_l_leftEye_lastSize;
    QSize d_l_rightEye_lastSize;
    QSize d_l_eyes_lastSize;
    QSize d_l_refractionLeft_lastSize;
    QSize d_l_refractionRight_lastSize;    
    QSize d_l_interocularRes_lastSize;
    QSize d_l_pic_FixLeft_lastSize;
    QSize d_l_pic_FixRight_lastSize;
    QSize d_measReviewButs_lastSize;

    //FOR TEST end

    QToolBar *d_topToolbar = nullptr;
    QPushButton d_but_close;
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
    QPixmap image(const Image_params &, QSize size);
    QPixmap ocular_pixmap(const Image_params & params, const EyeCirclePos &left, const EyeCirclePos &right);
    QPicture fixation_grid(int side, QColor grid = Qt::gray);
    QPicture fixation_result(const QPicture & grid, std::vector<EyeSkewCoords> skew_vec, QColor dots_color = Qt::red);
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    //void slot_diseaseTextChanged();

    void slot_idcursorPositionChanged(int oldPos, int newPos);
//    void slot_idEditingFinished();
//    void slot_idinputRejected();
//    void slot_idreturnPressed();
//    void slot_idselectionChanged();
//    void slot_idtextChanged(const QString &);
//    void slot_idtextEdited(const QString &);
    void slot_start();
    void slot_pwr();
    void slot_measure();
    void slot_showMeasImg(uint);
    //void slot_readUds(const UdsUniPack &pack);
    void slot_readUds(UdsUniPack pack);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    //bool event(QEvent*) override;
};
#endif // MAINWINDOW_H
