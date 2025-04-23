#include "mainwindow.h"
#include <QToolBar>
#include <QDebug>
#include <QIcon>
#include <QList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << Q_FUNC_INFO;

/*STYLES*/
    //STYLE strings
    QString str_dark_color_focusText = STR_MOSTDARK_COLOR;
    QString str_light_color_label = STR_MOSTLIGHT_COLOR;
    QString str_dark_color_label = STR_COLD_DARK_COLOR;    
    QString str_color_but = STR_WARM_DARK_COLOR;
    QString str_color_pressedBut = STR_WARM_LIGHT_COLOR;
    QString str_mostlight_color_bg = STR_MOSTLIGHT_COLOR;
    QString str_light_color_bg = STR_LIGHT_COLOR2;
    QString str_dark_color_bg = STR_COLD_DARK_COLOR;
    QString str_mostdark_color_bg = STR_MOSTDARK_COLOR;
    QString str_color_line = STR_COLD_DARK_COLOR;
    QString str_color_activeTab_bg = STR_WARM_DARK_COLOR;
    QString str_color_activeTab_text = STR_MOSTLIGHT_COLOR;
    QString str_color_notActiveTab_bg = STR_MOSTLIGHT_COLOR;
    QString str_color_notActiveTab_text = STR_WARM_DARK_COLOR;
    QString str_color_grid = STR_COLD_DARK_COLOR;
    QString str_color_dot = STR_WARM_DARK_COLOR;

    QString str_labelStyle_title = QString("background-color: transparent;"
                                           "color: %1;"
                                           "font: bold 15px;")
                                           .arg(str_light_color_label);
    QString str_lineStyle = QString("background-color: %1; ")
                                    .arg(str_color_line);
    QPalette palette_lineedit;
    palette_lineedit.setColor(QPalette::PlaceholderText,Qt::gray);
    palette_lineedit.setColor(QPalette::Text,QColor(str_dark_color_focusText));
    QString str_lineeditStyle = "border-style: none ; font: bold 12px; text-align: center;";
    QFont font_lineedit("Ubuntu Thin");
    Qt::Alignment align_lineedit = Qt::AlignLeft;
    QString str_dateEditButtonStyle = QString("QDateEdit"
                                              "{"
                                              "font: bold 12px; "
                                              "height: 15px;"
                                              "padding : 2px;"
                                              "color: %1;"
                                              //"image: url(:/img/down_butt.png);"
                                              "}"
                                              "QDateEdit::up-button {"
                                              "subcontrol-origin: border;"
                                              "subcontrol-position: top right;" /* position at the top right corner */

                                              "width: 30px;" /* 16 + 2*1px border-width = 15px padding + 3px parent border */
                                              "height:10px;"
                                              "border-image: url(:/img/up_butt.png) 1;"
                                              "border-width: 1px;}"
                                              "QDateEdit::down-button {"
                                              "subcontrol-origin: border;"
                                              "subcontrol-position: bottom right;" /* position at the top right corner */

                                              "width: 30px;" /* 16 + 2*1px border-width = 15px padding + 3px parent border */
                                              "height:10px;"
                                              "border-image: url(:/img/down_butt.png) 1;"
                                              "border-width: 1px;"
                                          "}"
                                              ).arg(str_dark_color_focusText);
    QString str_labelStyle = "color: gray; font: italic 12px";
    Qt::Alignment align_label = Qt::AlignLeft;
    QString str_butStyle = QString("QPushButton { background-color: %1; font: bold 15px;color: %3; border-radius: 1px;}"
                                   "QPushButton:pressed { background-color: %2; font: bold 20px;color: %3; border-radius: 1px;}")
                                   .arg(str_color_but)
                                   .arg(str_color_pressedBut)
                                   .arg(str_light_color_label);

    QString str_labelStyle_pic = QString("border-style: none ; color: %1").arg(str_dark_color_label);
    QString str_labelStyle_resultHeader = QString("border-style: none ; font: 15px; color: %1").arg(str_dark_color_label);
    QString str_labelStyle_resultData = QString("border-style: none ; font: 15px; color: %1").arg(str_dark_color_focusText);
    //    QString str_dateEditStyle = QString("border-style: none ;"
    //                                        "font: bold 15px; "
    //                                        //"padding-right: 15px;" /* make room for the arrows */
    //                                        "color: %1;"
    //                                        "height: 30px;")
    //                                        .arg(str_dark_color_focusText);
    //    setStyleSheet("QDateEdit{"
    //                  "font: bold 20px;}"
    //                  "QDateEdit::up-button {"
    //                  "border-width: 1px;"
    //                  //"width: 10px;"
    //                  //"height: 10px;};"
    //                  );
    //    QString str_dateEditButtonStyle = QString("QDateEdit::up-button { "
    //                                              "width:27px; "
    //                                              "height:21px;"
    //                                              //"background-color: %1; "
    //                                              //"bottom: 0px;}"
    //                                              //"QDateEdit::up-arrow {"
    //                                              //"image: url(:/img/up_arr.png);"
    //                                              //"background-color: %2"
    //                                              //"width:20px; "
    //                                              //"height:20px;}"
    //                                              "QDateEdit::down-button { "
    //                                              "width:27px; "
    //                                              "height:21px;"
    //                                              //"background-color: %1; "
    //                                              //"top: 0px;}"
    //                                              //"QDateEdit::down-arrow {"
    //                                              //"image: url(:/img/down_arr.png);"
    //                                              //"width:20px; "
    //                                              //"height:20px;}"
    //                                              )
    //            .arg(STR_WARM_DARK_COLOR);
    //    QString str_dateEditButtonStyle = QString("QDateEdit"
    //                                              "{"
    //                                              "height: 43px;"
    //                                              "padding : 5px;"
    //                                              //"image: url(:/img/down_butt.png);"
    //                                              "}"
    ////                                              "QDateEdit::up-button "
    ////                                              "{ "
    ////                                              "background-color : lightgreen; "
    ////                                              "}"
    //                                              "QDateEdit::up-arrow"
    //                                              "{"
    //                                              "image: url(:/img/close.png);"

    //                                              //"border : 2px solid black;"
    //                                              //"background-color : lightgreen;"
    //                                              "}"
    //                                              "QDateEdit::down-arrow"
    //                                              "{"
    //                                              //"border : 2px solid black;"
    //                                              //"background-color : red;"
    //                                              "image: url(:/img/down_butt.png);"
    //                                              "width:10px; "
    //                                              "height:10px;"
    //                                              "}"
    //                               )/*.arg(STR_WARM_DARK_COLOR)*/ ;
                //.arg(STR_MOSTLIGHT_COLOR);
    //        image: url(:/new/myapp/cbarrowdn.png);
    //        width:50px;
    //        height:15px;
    //        subcontrol-position: right top;
    //        subcontrol-origin:margin;
    //        background-color: white;
    //        border-style: solid;
    //        border-width: 4px;
    //        border-color: rgb(100,100,100);
    //       spacing: 5px;
    //    }

/*SIZE*/
    QSize screenSize = QGuiApplication::primaryScreen()->size();
    setBaseSize(screenSize);
    setFixedSize(screenSize);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    d_tab_central = new QTabWidget();
    d_tab_central->tabBar()->setExpanding(true);    
    setCentralWidget(d_tab_central);
/*set HOME tab*/
    //d_frame_home = new QFrame;
    /*set CARD tab*/
    d_frame_card = new QFrame;
    /*CARD layout*/
    QHBoxLayout *layout_card = new QHBoxLayout;
    /*layout with patient data & results*/
    QVBoxLayout *layout_data_and_results = new QVBoxLayout;
    /**********************************/
    /*frame & layout with patient data*/
    /**********************************/
    /*QFrame **/frame_data_total = new QFrame;
    QVBoxLayout *layout_data_total = new QVBoxLayout;
    /*Title*/
    QLabel *l_patientDataTitle = new QLabel(tr("Данные пациента"));
    l_patientDataTitle->setStyleSheet(str_labelStyle_title);
    layout_data_total->addWidget(l_patientDataTitle);
    QFrame *frame_data = new QFrame;
    QVBoxLayout *layout_data = new QVBoxLayout;
    QList<QLabel*> list_labels_caption;
    QList<QLineEdit*> list_lineedits_enterText;
//    /*first name*/
//    d_le_firstName = new QLineEdit;
//    list_lineedits_enterText << d_le_firstName;
//    d_le_firstName->setPlaceholderText(tr("Введите имя"));
//    layout_data->addSpacing(15);
//    layout_data->addWidget(d_le_firstName);
//    QFrame *line1 = new QFrame;
//    layout_data->addWidget(line1);
//    qDebug() << "line1->lineWidth()"<<line1->lineWidth();
//    qDebug() << "line1->width()"<<line1->width();
//    decorateLine(line1, str_lineStyle);
//    QLabel *l_firstName = new QLabel(tr("Имя"));
//    list_labels_caption << l_firstName;
//    //layout_data->addWidget(l_firstName);
//    layout_data->addSpacing(15);
//    layout_data->addStretch();
//    /*last name*/
//    d_le_lastName = new QLineEdit;
//    d_le_lastName->setPlaceholderText(tr("Введите фамилию"));
//    list_lineedits_enterText << d_le_lastName;
//    layout_data->addWidget(d_le_lastName);
//    QFrame *line2 = new QFrame;
//    decorateLine(line2, str_lineStyle);
//    layout_data->addWidget(line2);
//    QLabel *l_lastName = new QLabel(tr("Фамилия"));
//    list_labels_caption << l_lastName;
//    //layout_data->addWidget(l_lastName);
//    layout_data->addSpacing(15);
//    layout_data->addStretch();
//    /*middle name*/
//    d_le_middleName = new QLineEdit;
//    d_le_middleName->setPlaceholderText(tr("Введите отчество"));
//    list_lineedits_enterText << d_le_middleName;
//    layout_data->addWidget(d_le_middleName);
//    QFrame *line3 = new QFrame;
//    decorateLine(line3, str_lineStyle);
//    layout_data->addWidget(line3);
//    QLabel *l_fatherName = new QLabel(tr("Отчество"));
//    list_labels_caption << l_fatherName;
//    //layout_data->addWidget(l_fatherName);
//    layout_data->addSpacing(15);
//    layout_data->addStretch();
    /*ID*/
    QHBoxLayout* layout_id = new QHBoxLayout;
    QLabel *l_id = new QLabel(tr("ID"));
    //list_labels_caption << l_id;
    l_id->setStyleSheet(//"QGroupBox {"
                        //"background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
                //"border: 2px solid gray;"
                //"border-radius: 0px;"
                                   //"border-top-left-radius: 15px;"
                                   // "   border-top-right-radius: 15px;"
                //"margin-top: 1ex;"/* leave space at the top for the title */
                                   //"border-bottom-width: 3px;"
                                    //"   border-bottom-color: #528c83;"
                                      //" border-bottom-style: solid;" /* just a single line */
                //"};"

                //"    subcontrol-origin: margin;"
                //"    subcontrol-position: left;"//top center;" /* position at the top center */
                "    padding: 10px 10px;"
                "    background-color: #528c83;"
                                   "color: white;"
                "border-top-left-radius: 10px;"
                 "   border-bottom-left-radius: 10px;"
                "border-top-right-radius: 0px;"
                 "   border-bottom-right-radius: 0px;"
                 //"margin-left: 1ex;"                  //"margin-top: 1ex;" /* leave space at the top for the title */
                //"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FF0ECE, stop: 1 #FFFFFF); "
                 );
    layout_id->addWidget(l_id);
    d_le_id = new QLineEdit;
    d_le_id->setPlaceholderText(tr("Введите ID"));
    //list_lineedits_enterText << d_le_id;
    d_le_id->setStyleSheet("    padding: 10px 10px;"
                "border-bottom-width: 3px;"
                           "   border-bottom-color: #528c83;"
                           " border-bottom-style: solid;" /* just a single line */
                           "border-radius: 0px;");
    d_le_id->setInputMethodHints( Qt::ImhDigitsOnly);
    qDebug() << "ID connect" << connect(d_le_id, SIGNAL(cursorPositionChanged(int, int)), SLOT(slot_idcursorPositionChanged(int, int)));
    //qDebug() << "ID connect" << connect(d_le_id, SIGNAL(editingFinished()),                             SLOT(slot_idEditingFinished()));
    //qDebug() << "ID connect" << connect(d_le_id, SIGNAL(inputRejected()),                               SLOT(slot_idinputRejected()));
    //qDebug() << "ID connect" << connect(d_le_id, SIGNAL(returnPressed()),                               SLOT(slot_idreturnPressed()));
    //qDebug() << "ID connect" << connect(d_le_id, SIGNAL(selectionChanged()),                            SLOT(slot_idselectionChanged()));
    //qDebug() << "ID connect" << connect(d_le_id, SIGNAL(textChanged(const QString &)),                  SLOT(slot_idtextChanged(const QString &)));
    //qDebug() << "ID connect" << connect(d_le_id, SIGNAL(textEdited(const QString &)),                   SLOT(slot_idtextEdited(const QString &)));
    layout_id->addWidget(d_le_id);
    //layout_data->addWidget(d_le_id);
    layout_data->addLayout(layout_id);
    QFrame *line5 = new QFrame;
    decorateLine(line5, str_lineStyle);
    //layout_data->addWidget(line5);
    //QLabel *l_id = new QLabel(tr("ID"));
    //list_labels_caption << l_id;
    //layout_data->addWidget(l_id);
    //setStyle2list(list_lineedits_enterText, palette_lineedit, str_lineeditStyle, align_lineedit, font_lineedit);
    layout_data->addSpacing(15);
    layout_data->addStretch();

    /*SEX*/
    QGroupBox *gbx_sex = new QGroupBox(tr("Пол"));
    //QGroupBox {
    gbx_sex->setStyleSheet(//"QGroupBox {"
                //"background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #E0E0E0, stop: 1 #FFFFFF);"
        //"border: 2px solid gray;"
        //"border-radius: 0px;"
                           //"border-top-left-radius: 15px;"
                           // "   border-top-right-radius: 15px;"
        //"margin-top: 1ex;"/* leave space at the top for the title */
                           //"border-bottom-width: 3px;"
                            //"   border-bottom-color: #528c83;"
                              //" border-bottom-style: solid;" /* just a single line */
        //"};"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: left;"//top center;" /* position at the top center */
        "    padding: 10px 10px;"
        "    background-color: #528c83;"
                           "color: white;"
        "border-top-left-radius: 10px;"
         "   border-bottom-left-radius: 10px;"
         //"margin-left: 1ex;"                  //"margin-top: 1ex;" /* leave space at the top for the title */
        //"    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FF0ECE, stop: 1 #FFFFFF); "
        "}; "
                              );
    //};

//    QGroupBox::title {
//        subcontrol-origin: margin;
//        subcontrol-position: top center; /* position at the top center */
//        padding: 0 3px;
//        background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
//                                          stop: 0 #FF0ECE, stop: 1 #FFFFFF);
//    }
    d_female = new QRadioButton(tr("Женский"));
    d_male = new QRadioButton(tr("Мужской"));
    d_female->setChecked(true);
    QHBoxLayout* layout_sex = new QHBoxLayout;
    layout_sex->addSpacing(50);
    layout_sex->addWidget(d_female);
    layout_sex->addWidget(d_male);
    gbx_sex->setLayout(layout_sex);
    layout_data->addWidget(gbx_sex);
    QFrame *line1 = new QFrame;
    layout_data->addWidget(line1);
    qDebug() << "line1->lineWidth()"<<line1->lineWidth();
    qDebug() << "line1->width()"<<line1->width();
    decorateLine(line1, str_lineStyle);
    layout_data->addSpacing(15);
    layout_data->addStretch();
    /*birth date*/
    d_de_birthDate = new QDateEdit;
    d_de_birthDate->setInputMethodHints( Qt::ImhDigitsOnly);    
    d_de_birthDate->setButtonSymbols(QAbstractSpinBox::UpDownArrows);    
    //setStyleSheet(str_dateEditButtonStyle);
    ////d_de_birthDate->setStyleSheet(str_dateEditStyle);
    layout_data->addWidget(d_de_birthDate);
    QFrame *line4 = new QFrame;
    decorateLine(line4, str_lineStyle);
    layout_data->addWidget(line4);
    QLabel *l_birthDate = new QLabel(tr("Дата Рождения"));
    list_labels_caption << l_birthDate;
    //layout_data->addWidget(l_birthDate);
    layout_data->addSpacing(15);
    layout_data->addStretch();

    /*diseases*/
    d_pte_disease = new LineEdit_Keyboard();
    d_pte_disease->setPlaceholderText(tr("Введите сопутствующие заболевания"));
    //d_pte_disease->setStyleSheet(str_lineeditStyle);
    //d_pte_disease->setPalette(palette_lineedit);
    //d_pte_disease->setFont(font_lineedit);
    layout_data->addWidget(d_pte_disease);
    QFrame *line6 = new QFrame;
    decorateLine(line6, str_lineStyle);
    layout_data->addWidget(line6);
    QLabel *l_disease = new QLabel(tr("Cопутствующие заболевания"));
    list_labels_caption << l_disease;
    //layout_data->addWidget(l_disease);
    //layout_data->addSpacing(5);    
    setStyle2list(list_labels_caption, str_labelStyle, align_label);
    frame_data->setLayout(layout_data);
    layout_data_total->addWidget(frame_data);
    //layout_data_total->addStretch();
    frame_data_total->setLayout(layout_data_total);
    //layout_data_and_results->addWidget(frame_data_total);
    //layout_data_and_results->addStretch();
    /*PushButton START*/
    d_pb_start = new QPushButton (tr("Начать сеанс"));    
    //d_pb_start->setStyleSheet(str_butStyle);
    d_pb_start->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    d_pb_start->setFixedSize(300,40);
    #ifdef START_BY_ID
    d_pb_start->setDisabled(true);
    #endif
    //d_pb_start->setMinimumSize(120,30);
    //d_pb_start->setMaximumSize(300,50);
    //d_pb_start->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    qDebug() << connect(d_pb_start, SIGNAL(clicked()), SLOT(slot_start()));
    layout_data_and_results->addWidget(d_pb_start, 0, Qt::AlignHCenter);
    layout_data_and_results->addStretch();
    /*****************************/
    /*frame & layout with results*/
    /*****************************/
    /*QFrame **/frame_dataResults_total = new QFrame;
    QVBoxLayout *layout_dataResults_total = new QVBoxLayout;
    QLabel *l_patientResults = new QLabel(tr("Результаты измерений"));
    l_patientResults->setStyleSheet(str_labelStyle_title);
    layout_dataResults_total->addWidget(l_patientResults);
    QFrame *frame_dataResults = new QFrame;
    QHBoxLayout *layout_dataResults = new QHBoxLayout;
    d_l_pic_FixLeft = new QLabel;
    d_l_pic_FixRight = new QLabel;    
    d_l_pic_FixLeft->setStyleSheet(str_labelStyle_pic);
    d_l_pic_FixRight->setStyleSheet(str_labelStyle_pic);
    d_l_pic_FixRight->setAlignment(Qt::AlignRight);
    QVBoxLayout *layout_digitsDataResults = new QVBoxLayout;

    QLabel *l_refraction = new QLabel(tr("Рефракция, dpt"));
    l_refraction->setStyleSheet(str_labelStyle_resultHeader);
    l_refraction->setAlignment(Qt::AlignHCenter);
    layout_digitsDataResults->addWidget(l_refraction);
    QHBoxLayout *layout_refraction = new QHBoxLayout;
    d_l_refractionLeft = new QLabel;
    d_l_refractionRight = new QLabel;
    d_l_refractionRight->setAlignment(Qt::AlignRight);
    d_l_refractionLeft->setStyleSheet(str_labelStyle_resultData);
    d_l_refractionRight->setStyleSheet(str_labelStyle_resultData);
    layout_refraction->addWidget(d_l_refractionRight);
    layout_refraction->addSpacing(30);
    layout_refraction->addWidget(d_l_refractionLeft);
    layout_digitsDataResults->addLayout(layout_refraction);
    QFrame *line1_res = new QFrame;
    decorateLine(line1_res, str_lineStyle);
    layout_digitsDataResults->addWidget(line1_res);

    QLabel *l_interocular = new QLabel(tr("Межзрачковое расстояние, мм"));
    l_interocular->setAlignment(Qt::AlignHCenter);
    l_interocular->setStyleSheet(str_labelStyle_resultHeader);
    layout_digitsDataResults->addWidget(l_interocular);
    d_l_interocularRes = new QLabel;
    d_l_interocularRes->setAlignment(Qt::AlignHCenter);
    d_l_interocularRes->setStyleSheet(str_labelStyle_resultData);
    layout_digitsDataResults->addWidget(d_l_interocularRes);
    QFrame *line3_res = new QFrame;
    decorateLine(line3_res, str_lineStyle);
    layout_digitsDataResults->addWidget(line3_res);
    layout_digitsDataResults->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout_fixLeft = new QVBoxLayout;
    layout_fixLeft->setAlignment(Qt::AlignHCenter);
    QLabel *l_fixLeft = new QLabel(tr("OS"));
    l_fixLeft->setAlignment(Qt::AlignHCenter);
    l_fixLeft->setStyleSheet( str_labelStyle_resultHeader);
    layout_fixLeft->addWidget(l_fixLeft);
    layout_fixLeft->addWidget(d_l_pic_FixLeft);
    QVBoxLayout *layout_fixRight = new QVBoxLayout;
    layout_fixRight->setAlignment(Qt::AlignHCenter);
    QLabel *l_fixRight = new QLabel(tr("OD"));
    l_fixRight->setAlignment(Qt::AlignHCenter);
    l_fixRight->setStyleSheet( str_labelStyle_resultHeader);
    layout_fixRight->addWidget(l_fixRight);
    layout_fixRight->addWidget(d_l_pic_FixRight);
    layout_dataResults->addLayout(layout_fixRight);    
    layout_dataResults->addLayout(layout_digitsDataResults);
    layout_dataResults->addLayout(layout_fixLeft);

    frame_dataResults->setLayout(layout_dataResults);

    layout_dataResults_total->addWidget(frame_dataResults);
    layout_dataResults_total->addSpacing(10);
    d_l_error = new QLabel(STR_error);
    layout_dataResults_total->addWidget(d_l_error);
    layout_dataResults_total->addSpacing(30);
    d_l_extrametrics = new QLabel(STR_extrametrics);
    layout_dataResults_total->addWidget(d_l_extrametrics);
    layout_dataResults_total->addSpacing(30);
    d_l_strabismus = new QLabel(STR_strabismus);
    layout_dataResults_total->addWidget(d_l_strabismus);
    layout_dataResults_total->addSpacing(30);
    layout_dataResults_total->addStretch();
    frame_dataResults_total->setLayout(layout_dataResults_total);
    layout_data_and_results->addWidget(frame_dataResults_total);    
    layout_card->addLayout(layout_data_and_results);
    layout_data_and_results->addStretch();

    QVBoxLayout *layout_results = new QVBoxLayout;
    QHBoxLayout *layout_eyesResults = new QHBoxLayout;

    d_l_leftEye = new QLabel;
    d_l_rightEye= new QLabel;
    QString str_bgStyle_eye = QString("background-color: %1").arg(str_mostdark_color_bg);
    d_l_leftEye->setStyleSheet(str_bgStyle_eye);
    d_l_rightEye->setStyleSheet(str_bgStyle_eye);
    int screenWidth = QGuiApplication::primaryScreen()->size().width();
    qDebug() << "screenWidth"<<screenWidth;
    uint eyeSide = screenWidth *.25;
    d_l_leftEye->setFixedSize(eyeSide,eyeSide);
    d_l_rightEye->setFixedSize(eyeSide,eyeSide);
    qDebug() << "eyeSide"<<eyeSide;

    layout_eyesResults->addWidget(d_l_rightEye);
    layout_eyesResults->addWidget(d_l_leftEye);
    layout_results->addLayout(layout_eyesResults);
    d_l_eyes = new QLabel;
    d_l_eyes->setStyleSheet(str_bgStyle_eye);
    d_l_eyes->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    d_l_eyes->setFixedSize(eyeSide*2,eyeSide);
    layout_results->addWidget(d_l_eyes);

    //layout_results->addLayout(layout_dataResults);
    //layout_results->addStretch();
    layout_card->addLayout(layout_results);
    d_frame_card->setLayout(layout_card);    
    d_tab_central->insertTab(static_cast<int>(tabWidget::HOME), frame_data_total, tr("Дом"));
    d_tab_central->insertTab(static_cast<int>(tabWidget::CARD), d_frame_card, tr("Карточка"));
    qDebug() << "d_l_pic_FixLeft->size()"<<d_l_pic_FixLeft->size();
    //d_l_picLeftDataResults->setAlignment()
    //d_l_picLeftDataResults->setStyleSheet("border: 1px solid black ;"/*"background-color: black; color: white;"*/);
    int graphSide = (screenWidth - 2*eyeSide) / 5.;
    d_pic_fixGrid = fixation_grid(graphSide, str_color_grid);
    d_l_pic_FixLeft->setPicture(d_pic_fixGrid);
    d_l_pic_FixRight->setPicture(d_pic_fixGrid);
//#ifdef TEST_snapshot
//    std::vector<EyeSkewCoords> left(5);
//    std::vector<EyeSkewCoords> right(5);

//    for (size_t i = 0; i < 5; i++) {
//        left[i].x = i*5;
//        left[i].y = i*4;
//        right[i].x = i*2;
//        right[i].y = i*3;
//    }
//    d_l_pic_FixLeft->setFixedSize(d_pic_fixGrid.width(), d_pic_fixGrid.height());
//    d_l_pic_FixRight->setFixedSize(d_pic_fixGrid.width(), d_pic_fixGrid.height());
//    d_l_pic_FixLeft->setPicture(fixation_result(d_pic_fixGrid, left, STR_WARM_DARK_COLOR));
//    d_l_pic_FixRight->setPicture(fixation_result(d_pic_fixGrid, right, STR_WARM_DARK_COLOR));
//#endif
    d_l_pic_FixLeft->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);


    QFrame *frame_meas = new QFrame;
    QHBoxLayout *layout_meas = new QHBoxLayout;
    d_pb_shot = new QPushButton(tr("И\nз\nм\nе\nр\nе\nн\nи\nе"));
d_pb_shot->setFixedWidth(100);
    d_pb_shot->setSizePolicy(QSizePolicy::Preferred/*QSizePolicy::Fixed*/,QSizePolicy::Expanding);

    d_pb_shot->setStyleSheet(str_butStyle);

    qDebug() << connect(d_pb_shot, SIGNAL(clicked()), SLOT(slot_measure()));
    layout_meas->addWidget(d_pb_shot);

    layout_meas->addWidget(&d_l_snapshot);
    frame_meas->setLayout(layout_meas);
    d_tab_central->insertTab(static_cast<int>(tabWidget::MEAS), frame_meas, tr("Измерение"));
//setStyleSheet("QPushButton {color: #975536;background-color: #528c83;border-width: 0px;border-radius: 3px;}");
//setStyleSheet("background-color: #7eb7aa");
//setStyleSheet("QLabel {color: #528c83}; ");
//setStyleSheet("QDateEdit {color: #528c83}");
    //setStyleSheet("QLineEdit[text=""] { color: red; }");
////"qlineargradient(spread:reflect, x1:0.5, y1:0, x2:0, y2:0,"
////"stop:0 #7eb7aa, stop:1 #feda7c);");
//              "qlineargradient(x1:0, y1:0.5, x2:0, y2:0,"
//               "               stop:0 #a2d4df, "
//              "stop:1 #dddde2);");

//background
//d_frame_card->setStyleSheet("background-color: #80c8bc");
QString str_totalFrameStyle = QString("background-color: "
                                      "qlineargradient(x1:0, y1:0.2, x2:0, y2:0,"
                                      "stop:0 %1,"
                                      "stop:1 %2);")
                                .arg(str_light_color_bg)
                                .arg(str_dark_color_bg);
frame_data_total->setStyleSheet(str_totalFrameStyle);
frame_dataResults_total->setStyleSheet(str_totalFrameStyle);
QString str_frameStyle = QString("background-color: %1; border-radius: 10px;")
                                .arg(str_mostlight_color_bg);
frame_data->setStyleSheet(str_frameStyle);
frame_dataResults->setStyleSheet(str_frameStyle);
//frame_data->setStyleSheet("QPushButton {background-color: #528c83};");


//setStyleSheet("QTabWidget::pane { "/* The tab widget frame */
//    "border-top: 2px solid red;}"
////    "QTabWidget::tab-bar {"
////             " left: 5px;" /* move to the right by 5px */
//"}");
QString str_tabStyle = QString("QTabBar::tab:!selected {"
                               "background-color: %2;"
                               "color: %1;"
                               "min-width: 200px;"
                               "font-size: 15px;"
                               //"border-color:  %1;" /* make non-selected tabs look smaller */
                               "}"
                               "QTabBar::tab:selected {"
                               "background-color: %1;"
                               "color: %2;"
                               "min-width: 200px;"
                               "font: bold 15px;"
                               "}"
                               "QTabWidget::pane { "/* The tab widget frame */
                               "border-top: 3px solid %1;"
                               //"position: absolute;"
                               //"top: -0.5em;"
                               "}"
                               "QTabWidget::tab-bar {"
                               //"alignment: center;"
                               "left: 10px; " /* move to the right by 5px */
                           "}")
                                .arg(str_color_activeTab_bg)
                                .arg(str_color_activeTab_text);
setStyleSheet(str_tabStyle);
//setStyleSheet("QTabBar::tab {"
//              "background: "
//"     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
//"                                 stop: 0 red, stop: 0.4 #DDDDDD,"
//"                                 stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);"
//         "     border: 2px solid #C4C4C3;"
//         "     border-bottom-color: #C2C7CB;" /* same as the pane color */
//         "     border-top-left-radius: 4px;"
//         "     border-top-right-radius: 4px;"
//         "     min-width: 8ex;"
//         "     padding: 2px;"
//         " }"
//         "QTabBar::tab:selected, QTabBar::tab:hover {"
//         "     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
//         "                                 stop: 0 #fafafa, stop: 0.4 #f4f4f4,"
//         "                                 stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);"
//         " };");
//setStyleSheet("QMainWindow::close-button {image: url(:/img/next.png)}");


//QFrame *frame_fonts = new QFrame;
//QVBoxLayout *layout_fonts = new QVBoxLayout;
QFontDatabase base;
////qDebug()<< base.families()<<base.families().size();
//auto list = base.families();
//qDebug()<< list.size();
//QString str("Иванов Иван Иванович");
//for (int i = 40; i < 60 ; i++) {
//    QString text = str;
//    QLabel *l = new QLabel(text.append("  ").append(list.at(i)));
//    l->setFont(list.at(i));
//    layout_fonts->addWidget(l);
//    l->setStyleSheet("border-style: none ; font: bold 20px; ");

//    l->setPalette(p);
//    layout_data->addWidget(l);
//    qDebug()<< list.at(i);
//}
//QString text2 = str;
//QLabel *l2 = new QLabel(text2.append("  ").append(list.at(2)));
//l2->setFont(list.at(2));
//layout_fonts->addWidget(l2);
//QString text46 = str;
//QLabel *l46 = new QLabel(text2.append("  ").append(list.at(46)));
//l46->setFont(list.at(46));
//layout_fonts->addWidget(l46);
//QString text126 = str;
//QLabel *l126 = new QLabel(text2.append("  ").append(list.at(126)));
//l126->setFont(list.at(126));
//layout_fonts->addWidget(l126);
//QString text180 = str;
//QLabel *l180 = new QLabel(text2.append("  ").append(list.at(180)));
//l180->setFont(list.at(180));
//layout_fonts->addWidget(l180);
//frame_fonts->setLayout(layout_fonts);
//d_tab_central->insertTab(2, frame_fonts, tr("fonts"));



//#else
//    createToolBar();
//#endif
    d_topToolbar = new QToolBar;
    addToolBar(Qt::TopToolBarArea,d_topToolbar);
//d_but_close.setText("Закрыть");
    //
    //QIcon closeicon = QIcon::fromTheme("window-close");

    QIcon closeicon(":/img/close.png");
    d_but_close.setIcon(closeicon);
    d_but_close.setIconSize(QSize(35,35));
    d_but_close.setStyleSheet(//"color: grey;"
                              //"border-image: url(:/img/close.png) 50 50 50 50;"
                              "border-top: transparent;"
                              "border-bottom: transparent;"
                              "border-right: transparent;"
                              "border-left: transparent;"
                              );
    //QGraphicsColorizeEffect eff;
    //eff.setColor(QColor(STR_WARM_DARK_COLOR));
    //eff.setColor(Qt::blue);
    //d_but_close.setGraphicsEffect(&eff);

    //QWidget* dummy = new QWidget();
    QLabel* title = new QLabel(STR_TITLE);
    title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    title->setFont(base.families().at(2));
    //QString str_labelStyle_wintitle = QString("border-style: none ; color: %1; ").arg(str_dark_color_label);
    QString str_labelStyle_wintitle = QString("font: bold 14px; color: red; ").arg(str_dark_color_label);//font: bold 14px;
    title->setStyleSheet(str_labelStyle_wintitle);
    d_topToolbar->addWidget(title);

    connect(&d_but_close, SIGNAL(clicked()), SLOT(close()));
    d_topToolbar->addWidget(&d_but_close);
    d_topToolbar->setMovable(false);
    showFullScreen();
    //resize(1500,1500);
    //showMaximized();
    //setWindowFlag(Qt::FramelessWindowHint);
    //setCentralWidget( &d_l_snapshot);

    d_l_snapshot.setStyleSheet(str_bgStyle_eye);
    d_measReviewButs = new ImageButtons(&d_l_snapshot);
    //d_measReviewButs->setVisible(false);
    d_measReviewButs->hide(true);
    //d_measReviewButs->hide(false);
    connect(d_measReviewButs, SIGNAL(showMeasImg_clicked(uint)), SLOT(slot_showMeasImg(uint)));

    d_l_snapshot.setAlignment(Qt::AlignCenter);

    initNetwork();

    d_measResultShmemReader = std::make_unique<MeasResultShmemReader>();
    if(d_measResultShmemReader->setup() < 0){
        qDebug() << "MeasResultShmemReader setup fail";
        d_measResultShmemReader.reset();
    }
    qDebug() << Q_FUNC_INFO << "end";
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

//void MainWindow::createToolBar()
//{
//    qDebug() << Q_FUNC_INFO;
//    //setStyleSheet("QPushButton { color: red; spacing: 20px;  } ");
//    d_toolbar = new EMToolBar(this);
//    addToolBar(Qt::LeftToolBarArea,d_toolbar);
//    qDebug() << connect(d_toolbar, SIGNAL(sig_startTriggered()), SLOT(slot_start()));
//    qDebug() << connect(d_toolbar, SIGNAL(sig_pwrTriggered()), SLOT(slot_pwr()));
//    qDebug() << connect(d_toolbar, SIGNAL(sig_measureTriggered()), SLOT(slot_measure()));
//}

QString MainWindow::savingPath()
{
    qDebug() << Q_FUNC_INFO;
    QFile file("settings.conf");
    QString path;
    if(file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);        
        in >> path;
        qDebug()<<"saving path:"<<path;
        file.close();
    }
    qDebug() << Q_FUNC_INFO << "end";
    return path;
}

//void MainWindow::slot_diseaseTextChanged()
//{
//    qDebug() << Q_FUNC_INFO;
//    QSize size = d_pte_disease->document()->size().toSize();
//    d_pte_disease->setMaximumHeight( size.height() + 3 );
//}


void MainWindow::slot_idcursorPositionChanged(int oldPos, int newPos)
{
    qDebug() << Q_FUNC_INFO;
    if(!d_le_id->text().isEmpty()){
        d_pb_start->setEnabled(true);
        qDebug() << "d_le_id->text()" << d_le_id->text();
    }
}
//void MainWindow::slot_idEditingFinished()
//{
//    qDebug() << Q_FUNC_INFO;
//    d_pb_start->setEnabled(true);
//}
//void MainWindow::slot_idinputRejected()
//{
//    qDebug() << Q_FUNC_INFO;
//}
//void MainWindow::slot_idreturnPressed()
//{
//    qDebug() << Q_FUNC_INFO;
//    d_pb_start->setEnabled(true);
//}
//void MainWindow::slot_idselectionChanged()
//{
//    qDebug() << Q_FUNC_INFO;
//    d_pb_start->setEnabled(true);
//}
//void MainWindow::slot_idtextChanged(const QString &)
//{
//    qDebug() << Q_FUNC_INFO;
//    d_pb_start->setEnabled(true);
//}
//void MainWindow::slot_idtextEdited(const QString &)
//{
//    qDebug() << Q_FUNC_INFO;
//    d_pb_start->setEnabled(true);
//}

void MainWindow::slot_start()
{
    qDebug() << Q_FUNC_INFO;
    if(d_udsUniSocket == nullptr)
        return;
    d_udsUniSocket->send(UdsUniTitle::UDSUNI_TITLE_STREAM_START);
    if(d_measReviewButs != nullptr)
        d_measReviewButs->hide(true);
#ifdef TEST_snapshot
    if(d_measReviewButs != nullptr)
    {
    d_measReviewButs->resize(d_l_snapshot.width(),d_l_snapshot.height());
    d_measReviewButs->hide(false);
    //d_measReviewButs->setImageCount(d_vec_snapshots.size());
    }
    d_snapshotParams.frame_height = 1216;
    d_snapshotParams.frame_width = 1936;
    d_snapshotParams.size = d_snapshotParams.frame_height * d_snapshotParams.frame_width;
    d_snapshotParams.buf.resize(d_snapshotParams.size);
    //uchar buf[d_snapshotParams.size];
    qDebug() << Q_FUNC_INFO << "file_0";
    FILE *f; // переменная для работы с файлом

    //f=fopen("/home/moonlight/Main/Work/eyemeter/repa/EyeMeter/eyemetergui/frame1.bin", "r"); // открываем бинарный файл для записи и чтения в режиме добавления, то есть, если файла нет, то он создастся, а если файл есть, то содержимое файла не будет уничтожено, из файла можно будет читать и в файл можно будет записывать
    f=fopen("/home/moonlight/Main/Work/eyemeter/repa/EyeMeter/eyemetergui/_2024_08_08_11_19_40.bin", "r");
    qDebug() << Q_FUNC_INFO << f;
    qDebug() << fread(d_snapshotParams.buf.data() , sizeof(uchar), d_snapshotParams.size, f); // считываем из файла f ровно 1 пакет pack размера int_double

    fclose(f);
    qDebug() << Q_FUNC_INFO << "file_end";
    //memcpy(d_snapshotParams.buf, &buf, d_snapshotParams.size);
    QPixmap pix = image(d_snapshotParams, d_l_snapshot.size());
    //double kscale = static_cast<double>(pix.width())/d_snapshotParams.frame_width;
    //qDebug() << "kscale" << kscale;
    //qDebug() << "hscale" << static_cast<double>(pix.height())/d_snapshotParams.frame_height;
    d_l_snapshot.setPixmap(pix);
    //{'n_frame': 36, 'left_x': 1386, 'left_y': 529, 'left_r': 26, 'right_x': 822, 'right_y': 569, 'right_r': 26}}
    EyeCirclePos left {1386,529,26};
    EyeCirclePos right {822,569,26};
    qDebug() << "d_l_eyes->size()" << d_l_eyes->size();
    if(!d_isEyesLabelSizeFixed)
    {
        d_eyesLabelSize = d_l_eyes->size();
        d_l_eyes->setFixedSize(d_eyesLabelSize);
        d_isEyesLabelSizeFixed = true;
    }
    d_l_eyes->setPixmap(ocular_pixmap(d_snapshotParams, left, right)
           .scaled(d_eyesLabelSize,Qt::KeepAspectRatio));
    qDebug() << "d_l_eyes->size()" << d_l_eyes->size();
    //d_l_snapshot.adjustSize();
    std::vector<EyeSkewCoords> leftSkew(5);
    std::vector<EyeSkewCoords> rightSkew(5);

    for (size_t i = 0; i < 5; i++) {
       leftSkew[i].x = i*5;
       leftSkew[i].y = 0;
       rightSkew[i].x = i*2;
       rightSkew[i].y = 10;
    }
    d_l_pic_FixLeft->setFixedSize(d_pic_fixGrid.width(), d_pic_fixGrid.height());
    d_l_pic_FixRight->setFixedSize(d_pic_fixGrid.width(), d_pic_fixGrid.height());
    d_l_pic_FixLeft->setPicture(fixation_result(d_pic_fixGrid, leftSkew, STR_WARM_DARK_COLOR));
    d_l_pic_FixRight->setPicture(fixation_result(d_pic_fixGrid, rightSkew, STR_WARM_DARK_COLOR));

    AIEyeMeasResult measResult;
    measResult.left.sphere = 1.3;
    measResult.left.cylinder = 2.3;
    measResult.left.angle = 3.3;
    measResult.left.diameter = 4.3;
    measResult.right.sphere = 5.3;
    measResult.right.cylinder = 5.3;
    measResult.right.angle = 6.3;
    measResult.right.diameter = 7.3;
    measResult.error_word = 64;
    measResult.left_sharpness = 1;
    measResult.right_sharpness = 2;
    measResult.left_flick_intensity = 3;
    measResult.right_flick_intensity = 4;
    measResult.strabismus = 5.5;
    QString measResLeft_str = QString("%1   %2   %3º   %4")
            .arg(measResult.left.sphere)   //2
            .arg(measResult.left.cylinder) //3
            .arg(measResult.left.angle)    //4
            .arg(measResult.left.diameter); //5
    QString measResRightt_str = QString("%1   %2   %3º   %4")
            .arg(measResult.right.sphere)   //6
            .arg(measResult.right.cylinder) //7
            .arg(measResult.right.angle)    //8
            .arg(measResult.right.diameter); //9

    d_l_refractionLeft->setText(measResLeft_str);
    d_l_refractionRight->setText(measResRightt_str);
    d_l_interocularRes->setText("10");
    std::wstring error_text;
    std::wstring error_description;
    qDebug()<<"eyeMeasResultErrorText";
    if(eyeMeasResultErrorText(measResult.error_word,EYEGUILANG_RUS,error_text,error_description))
    {
         qDebug() << QString::fromStdWString(error_text);
        d_l_error->setText(STR_error+": "+QString::fromStdWString(error_text));
    }
    QString extrametrics_str = QString("%1: %2, %3, %4, %5")
            .arg(STR_extrametrics)
            .arg(measResult.left_sharpness)
            .arg(measResult.right_sharpness)
            .arg(measResult.left_flick_intensity)
            .arg(measResult.right_flick_intensity);
    d_l_extrametrics->setText(extrametrics_str);    
    QString strabismus_str = QString("%1: %2")
            .arg(STR_strabismus)
            .arg(measResult.strabismus);
    d_l_strabismus->setText(strabismus_str);
    measFinished("TEST");
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
    //d_vec_snapshots.reserve(CONST_MEASURE_SHOTS_COUNT);
    if(d_measReviewButs != nullptr)
    {
        d_measReviewButs->hide(true);
        d_measReviewButs->resize(d_l_snapshot.width(),d_l_snapshot.height());
    }
    d_isMeasurStarted = false;
    /*Clear results*/
    d_l_pic_FixLeft->setPicture(d_pic_fixGrid);
    d_l_pic_FixRight->setPicture(d_pic_fixGrid);
    d_l_refractionLeft->clear();
    d_l_refractionRight->clear();
    d_l_interocularRes->clear();
    d_l_leftEye->clear();
    d_l_rightEye->clear();
    d_l_eyes->clear();
    d_l_error->clear();
    d_l_extrametrics->clear();
    d_l_strabismus->clear();
}

void MainWindow::slot_showMeasImg(uint num)
{
    qDebug() << Q_FUNC_INFO;
    if(d_vec_snapshots.size()>static_cast<int>(num))
    {
        QPixmap pix = image( d_vec_snapshots.at(num), d_l_snapshot.size());
        qDebug() << "d_l_snapshot pix.size()" << pix.size();
        d_l_snapshot.setPixmap(pix);
        qDebug() << "d_l_snapshot pix.size()" << pix.size();
        //d_l_snapshot.adjustSize();
    }
    qDebug() << Q_FUNC_INFO << "end";
}

void MainWindow::initNetwork()
{
    qDebug() << Q_FUNC_INFO;
    d_udsUniSocket = new UdsUniSocket();
    qDebug() << d_udsUniSocket->isValid();
    if(!d_udsUniSocket->isValid())
        return;
    qDebug() << connect(d_udsUniSocket, SIGNAL(readyRead(UdsUniPack)), SLOT(slot_readUds(UdsUniPack)));
}

void MainWindow::slot_readUds(UdsUniPack pack)
{
    qDebug() << Q_FUNC_INFO;
    if(pack.msg.proto != UDSUNI_PROTO_PTTS4)
        return;
    switch (pack.msg.title) {
    case UDSUNI_TITLE_STREAM_RUNNING:
        qDebug() << "UDSUNI_TITLE_STREAM_RUNNING";
        d_tab_central->setCurrentIndex(static_cast<int>(tabWidget::MEAS));
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
            if(res==-1)
            {;}//????????????????
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
                //qDebug() << "UDSUNI_TITLE_FRAME_READY 0";
                if(d_shmemBlockReader->get_block(block) < 0)
                    break;
                //qDebug() << "UDSUNI_TITLE_FRAME_READY 1";
                if(block.ptr == nullptr || block.size == 0)
                    break;
                memcpy(d_snapshotParams.buf.data(), block.ptr, block.size);
                //qDebug() << "UDSUNI_TITLE_FRAME_READY 2";
                qDebug() << "d_l_snapshot.size()" << d_l_snapshot.size();
                QPixmap pix = image(d_snapshotParams, d_l_snapshot.size());
                //qDebug() << "UDSUNI_TITLE_FRAME_READY 3";
                d_l_snapshot.setPixmap(pix);
                qDebug() << "d_l_snapshot.size()" << d_l_snapshot.size();
                d_l_snapshot.setFixedSize(d_l_snapshot.size());
                if(d_isMeasurStarted)
                    d_vec_snapshots.push_back(d_snapshotParams);
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
                d_measShotsCount = settings.n_led_pos * settings.n_repeat;
                d_vec_snapshots.reserve(d_measShotsCount);
//                d_udsUniSocket->send(UDSUNI_TITLE_FRAME_FREE, frame.id, EYEMETER_ROLE_CAM);           
        }
        break;
    case UDSUNI_TITLE_MEAS_SHOOT_DONE:
    {
        qDebug() << "UDSUNI_TITLE_MEAS_SHOOT_DONE";
#ifdef START_BY_ID
        d_pb_start->setDisabled(true);
#endif
        //d_isMeasurStarted = false;
        QString file_measure_str = d_le_id->text().append('_').append(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss")).append(".bin");
        qDebug() << "file_measure: " << file_measure_str;
        QString filepath = QString("%1/%2").arg(savingPath()).arg(file_measure_str);
        d_file_measure.setFileName(filepath);
        if(d_file_measure.open(QIODevice::WriteOnly | QIODevice::Append))
        {
            foreach (Image_params shot, d_vec_snapshots) {
                d_file_measure.write(shot.buf.data(),shot.size);
            }
        }        
        d_file_measure.flush();
        d_file_measure.close();

        measFinished(tr("СНЯТО"));
        if(d_measReviewButs != nullptr)
        {            
            d_measReviewButs->hide(false);
            d_measReviewButs->setImageCount(d_vec_snapshots.size());
        }
        //d_vec_snapshots.clear(); //don't clear! it is used by d_measReviewButs
        qDebug() << "UDSUNI_TITLE_MEAS_SHOOT_DONE end";
    }
        break;
    case UDSUNI_TITLE_MEAS_RESULT_FAILED:
        qDebug() << "UDSUNI_TITLE_MEAS_RESULT_FAILED";        
        measFinished(tr("Ошибка измерения"));//+описание
        break;
    case UDSUNI_TITLE_MEAS_FAILED:
        qDebug() << "UDSUNI_TITLE_MEAS_FAILED";
        measFinished(tr("Ошибка"));
        break;
    case UDSUNI_TITLE_MEAS_RESULT:
    {
        qDebug() << "UDSUNI_TITLE_MEAS_RESULT";
        AIEyeMeasResult measResult;
        pack.fetch_data(measResult);

        QString measResLeft_str = QString("%1   %2   %3º   %4")
                .arg(measResult.left.sphere)    //1
                .arg(measResult.left.cylinder)  //2
                .arg(measResult.left.angle)     //3
                .arg(measResult.left.diameter); //4
        QString measResRight_str = QString("%1   %2   %3º   %4")
                .arg(measResult.right.sphere)    //1
                .arg(measResult.right.cylinder)  //2
                .arg(measResult.right.angle)     //3
                .arg(measResult.right.diameter); //4

        d_l_refractionLeft->setText(measResLeft_str);
        d_l_refractionRight->setText(measResRight_str);

        d_l_interocularRes->setText(QString::number(measResult.interocular));        

        std::wstring error_text;
        std::wstring error_description;

        if(eyeMeasResultErrorText(measResult.error_word,EYEGUILANG_RUS,error_text,error_description))
        {
             qDebug() << QString::fromStdWString(error_text);
            d_l_error->setText(STR_error+": "+QString::fromStdWString(error_text));
        }
        QString extrametrics_str = QString("%1: %2, %3, %4, %5")
                .arg(STR_extrametrics)
                .arg(measResult.left_sharpness)
                .arg(measResult.right_sharpness)
                .arg(measResult.left_flick_intensity)
                .arg(measResult.right_flick_intensity);
        d_l_extrametrics->setText(extrametrics_str);
        QString strabismus_str = QString("%1: %2")
                .arg(STR_strabismus)
                .arg(measResult.strabismus);
        d_l_strabismus->setText(strabismus_str);
        if(d_measResultShmemReader != nullptr)
        {
            /*SKEW*/
            std::vector<EyeSkewCoords> leftSkew;
            std::vector<EyeSkewCoords> rightSkew;
            d_measResultShmemReader->get_skew(leftSkew, rightSkew, d_measShotsCount);
            d_l_pic_FixLeft->setFixedSize(d_pic_fixGrid.width(), d_pic_fixGrid.height());
            d_l_pic_FixRight->setFixedSize(d_pic_fixGrid.width(), d_pic_fixGrid.height());
            d_l_pic_FixLeft->setPicture(fixation_result(d_pic_fixGrid, leftSkew, STR_WARM_DARK_COLOR));
            d_l_pic_FixRight->setPicture(fixation_result(d_pic_fixGrid, rightSkew, STR_WARM_DARK_COLOR));
            /*PUPILS*/
            d_measResultShmemReader->get_pupils(d_leftPupil, d_rightPupil);
            Image_params pupilImageParams_left;
            pupilImageParams_left.frame_height = d_leftPupil.height;
            pupilImageParams_left.frame_width = d_leftPupil.width;
            pupilImageParams_left.size = d_leftPupil.size;
            pupilImageParams_left.buf.resize(pupilImageParams_left.size);
            if(d_leftPupil.data_ptr == nullptr || d_leftPupil.size == 0)
                break;
            memcpy(pupilImageParams_left.buf.data(), d_leftPupil.data_ptr, d_leftPupil.size);
            QPixmap pix_left = image(pupilImageParams_left, d_l_leftEye->size());
            d_l_leftEye->setPixmap(pix_left);
            Image_params pupilImageParams_right;
            pupilImageParams_right.frame_height = d_rightPupil.height;
            pupilImageParams_right.frame_width = d_rightPupil.width;
            pupilImageParams_right.size = d_rightPupil.size;
            pupilImageParams_right.buf.resize(pupilImageParams_right.size);
            if(d_rightPupil.data_ptr == nullptr || d_rightPupil.size == 0)
                break;
            memcpy(pupilImageParams_right.buf.data(), d_rightPupil.data_ptr, d_rightPupil.size);
            QPixmap pix_right = image(pupilImageParams_right, d_l_rightEye->size());
            d_l_rightEye->setPixmap(pix_right);
            /*OCULARS*/
            qDebug() << "d_vec_snapshots.size()" << d_vec_snapshots.size();
            qDebug() << "measResult.frame4circles" << measResult.frame4circles;
            if(d_vec_snapshots.size() > static_cast<int>(measResult.frame4circles))
            {
                qDebug() << "d_vec_snapshots.size() > static_cast<int>(measResult.frame4circles)";
                //QPixmap snapshot = image( d_vec_snapshots.at(measResult.frame4circles), d_l_snapshot.size());
                //double kscale = static_cast<double>(snapshot.width())/d_vec_snapshots.at(measResult.frame4circles).frame_width;
                qDebug() << "d_l_eyes->size()" << d_l_eyes->size();
                if(!d_isEyesLabelSizeFixed)
                {
                    d_eyesLabelSize = d_l_eyes->size();
                    d_l_eyes->setFixedSize(d_eyesLabelSize);
                    d_isEyesLabelSizeFixed = true;
                }
                d_l_eyes->setPixmap(ocular_pixmap(d_vec_snapshots.at(measResult.frame4circles), measResult.left.position, measResult.right.position)
                        .scaled(d_eyesLabelSize,Qt::KeepAspectRatio));
                qDebug() << "d_l_eyes->size()" << d_l_eyes->size();
            }
            else
            {
                qDebug() << "d_vec_snapshots.size() <= static_cast<int>(measResult.frame4circles)";
            }

        }

    }
        break;
    default:
        break;
    }
    pack.clear_data();

    qDebug()<<Q_FUNC_INFO << "end";
}

QPixmap MainWindow::image(const Image_params &imgParams, QSize size)
{
    qDebug() << Q_FUNC_INFO;
    int bytesPerLine = imgParams.frame_width;
    qDebug() << "imgParams.frame_width" << imgParams.frame_width;
    qDebug() << "imgParams.frame_height" << imgParams.frame_height;
    QImage snapshot_img((uchar*)imgParams.buf.c_str(), imgParams.frame_width, imgParams.frame_height, bytesPerLine, QImage::Format_Grayscale8 /*QImage::Format_Alpha8 QImage::Format_Indexed8, QImageCleanupFunction cleanupFunction = nullptr, void *cleanupInfo = nullptr*/);
    qDebug() << "snapshot_img.size()" << snapshot_img.size();
    //qDebug() << "d_l_snapshot.size()" << d_l_snapshot.size();
    QPixmap pix = QPixmap::fromImage(snapshot_img.scaled(size, Qt::KeepAspectRatio, Qt::FastTransformation));
    qDebug() << "pix_snapshot.size()" << pix.size();
qDebug() << "d_l_snapshot.size()" << d_l_snapshot.size();
qDebug() << "d_l_leftEye"<<d_l_leftEye->size();
qDebug() << "d_l_rightEye"<<d_l_rightEye->size();
qDebug() << "d_l_eyes"<<d_l_eyes->size();
qDebug() << "d_l_pic_FixLeft"<<d_l_pic_FixLeft->size();
qDebug() << "d_l_pic_FixRight"<<d_l_pic_FixRight->size();
qDebug() << "d_l_refractionLeft"<<d_l_refractionLeft->size();
qDebug() << "d_l_refractionRight"<<d_l_refractionRight->size();
qDebug() << "d_l_interocularRes"<<d_l_interocularRes->size();

    //qDebug() << "d_l_snapshot.size()" << d_l_snapshot.size();
//    QPainter painter;
//    painter.begin(&pix);
//    painter.setPen(QPen(QColor(STR_COLD_DARK_COLOR), 5));
//    QPoint points1[4];
//    points1[0] = QPoint(50,150);
//    points1[1] = QPoint(50,50);
//    points1[2] = QPoint(50,50);
//    points1[3] = QPoint(150,50);
//    QPoint points2[4];
//    points2[0] = QPoint(pix.width()-50,150);
//    points2[1] = QPoint(pix.width()-50,50);
//    points2[2] = QPoint(pix.width()-50,50);
//    points2[3] = QPoint(pix.width()-150,50);
//    QPoint points3[4];
//    points3[0] = QPoint(50,pix.height()-150);
//    points3[1] = QPoint(50,pix.height()-50);
//    points3[2] = QPoint(50,pix.height()-50);
//    points3[3] = QPoint(150,pix.height()-50);
//    QPoint points4[4];
//    points4[0] = QPoint(pix.width()-50,pix.height()-150);
//    points4[1] = QPoint(pix.width()-50,pix.height()-50);
//    points4[2] = QPoint(pix.width()-50,pix.height()-50);
//    points4[3] = QPoint(pix.width()-150,pix.height()-50);
//    painter.drawLines(points1,2);
//    painter.drawLines(points2,2);
//    painter.drawLines(points3,2);
//    painter.drawLines(points4,2);
//    painter.end();
    return pix;
}

void MainWindow::measFinished(const QString &res)
{
    qDebug() << Q_FUNC_INFO;
    d_isMeasurStarted = false;
    //d_file_measure.close();
    ///*int ret = */QMessageBox::information(this, tr("Измерения завершены."),
    //                                       res,
    //                                       QMessageBox::Ok,
    //                                       QMessageBox::Ok);
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Измерения завершены."));
    msgBox.setText(res);
    //msgBox.setIcon(QMessageBox::Information);
                //msgBox.setParent(QWidget Ui->QMainWindow);
    QSize screenSize = QGuiApplication::primaryScreen()->size();
    //msgBox.resize(600,400);
                msgBox.setGeometry(screenSize.width() - 200, 450, 200, 200);
    qDebug() << msgBox.width();
    msgBox.exec();
    d_tab_central->setCurrentIndex(static_cast<int>(tabWidget::CARD));
}

void MainWindow::setPhotoScreen()
{

}

QPicture MainWindow::fixation_grid(int side, QColor grid)
{
    qDebug() << Q_FUNC_INFO;
    QPainter painter;
    QPicture pic;
    painter.begin(&pic);

    //Grid
    painter.setPen(QPen(grid, 1, Qt::DashLine));
    //qreal rad1
//    painter.drawEllipse(QPointF(side/2., side/2), side/6., side/6.);
//    painter.drawEllipse(QPointF(side/2., side/2), side/3., side/3.);
//    //painter.drawRect(0,0,side-1,side-1);
//    painter.drawLine(side/2.,0, side/2., side);
//    painter.drawLine(0, side/2., side, side/2.);
//    painter.drawLine(0, 0, side, side);
//    painter.drawLine(0, side, side, 0);
    painter.drawEllipse(QPointF(0, 0), side/6., side/6.);
    painter.drawEllipse(QPointF(0, 0), side/3., side/3.);
    //painter.drawRect(0,0,side-1,side-1);
    painter.drawLine(-1*side/2.,-1*side/2., side/2., side/2.);
    painter.drawLine(0, -1*side/2., 0, side/2.);
    painter.drawLine(-1*side/2., 0, side/2., 0);
    painter.drawLine(-1*side/2., side/2., side/2., -1*side/2.);

    //Labels
    //Distances
    painter.setPen(QPen(grid, 3));
    //painter.setBrush(QBrush(bgColor));
    QFont font;
    font.setPixelSize(12);
    painter.setFont(font);
    double textOffset = 2.;

    int tickNum = 3;
    for (int k = 0; k < tickNum; k++)
    {
        QStaticText stext(QString::number(k*10));
        QSizeF textSize = stext.size();
        QPointF topLeftPosition(textOffset, side/6. * k + textOffset);
        QRectF textBgRect(topLeftPosition,textSize);
        //painter.setPen(Qt::transparent);
        //painter.drawRect(textBgRect);
        //painter.setPen(QPen(MTD::labelColor));
        painter.drawText(textBgRect,QString::number(k*10));
    }
    painter.end();
    return pic;
}

QPicture MainWindow::fixation_result(const QPicture &grid, std::vector<EyeSkewCoords> skew_vec, QColor dots_color)
{
    qDebug() << Q_FUNC_INFO;
    QPainter painter;
    QPicture pic;
    painter.begin(&pic);
    painter.drawPicture(0,0, grid);
    size_t s = skew_vec.size();
    QPointF left_arr[s];
    float k = grid.height()/60.;
    for (size_t i = 0; i < s; i++) {
        left_arr[i] = QPointF(skew_vec.at(i).x *k, skew_vec.at(i).y *k);
    }
    painter.setPen(QPen(dots_color, 7, Qt::SolidLine, Qt::RoundCap));
    painter.drawPoints(left_arr,s);
    painter.end();
    return pic;
}
QPixmap MainWindow::ocular_pixmap(const Image_params &imgParams, const EyeCirclePos &left, const EyeCirclePos &right)
{
    qDebug() << Q_FUNC_INFO;
    int bytesPerLine = imgParams.frame_width;
    qDebug() << "imgParams.frame_width" << imgParams.frame_width;
    qDebug() << "imgParams.frame_height" << imgParams.frame_height;
    QImage snapshot_img((uchar*)imgParams.buf.c_str(), imgParams.frame_width, imgParams.frame_height, bytesPerLine, QImage::Format_Grayscale8 /*QImage::Format_Alpha8 QImage::Format_Indexed8, QImageCleanupFunction cleanupFunction = nullptr, void *cleanupInfo = nullptr*/);
    QPixmap pix = QPixmap::fromImage(snapshot_img);
    //unsigned short midline = static_cast<unsigned short>(round((left.vert + right.vert) / 2.));
    qDebug() << "left.horiz" << left.horiz;
    qDebug() << "left.vert" << left.vert;
    qDebug() << "left.radius" << left.radius;
    qDebug() << "right.horiz" << right.horiz;
    qDebug() << "right.vert" << right.vert;
    qDebug() << "right.radius" << right.radius;
    double midline = (left.vert + right.vert) / 2.;
    qDebug() << "midline" << midline;
    //midline = (midline <= (frame.height()-midline)) ? midline : (frame.height()-midline);
    //qDebug() << "midline" << midline;
    if(left.vert < left.radius
            || right.vert < right.radius
            || pix.height() < (left.vert + left.radius)
            || pix.height() < (right.vert + right.radius)
            || left.horiz < left.radius
            || pix.width() < (left.horiz + left.radius)
            || right.horiz < right.radius
            || pix.width() < (right.horiz + right.radius) )
       return QPixmap();

    //QPixmap pix(params);
    QPainter painter;
    painter.begin(&pix);
    painter.setPen(QPen(QColor(STR_COLD_DARK_COLOR), 2));
    painter.drawEllipse(QPointF(left.horiz,left.vert),left.radius,left.radius);
    painter.drawEllipse(QPointF(right.horiz,right.vert),right.radius,right.radius);
    if(left.horiz < right.horiz)
        painter.drawLine(QPointF((left.horiz + left.radius),left.vert),QPointF((right.horiz - right.radius),right.vert));
    else
        painter.drawLine(QPointF((right.horiz + right.radius),right.vert),QPointF((left.horiz - left.radius),left.vert));
    //painter.setPen(QPen(QColor(Qt::red), 2));
    //painter.drawRect(pix.rect());
    //qDebug() << "pix.rect()" << pix.rect();
    painter.end();
    //d_l_snapshot.setPixmap(pix.scaled(d_l_snapshot.size(), Qt::KeepAspectRatio, Qt::FastTransformation));
    qDebug() << "ocular_pixmap" << 1;
    QRect rect;

    qDebug() << "frame.size()" << pix.size();
    qDebug() << "d_l_snapshot.size()" << d_l_snapshot.size();
    qDebug() << "d_l_eyes.size()" << d_l_eyes->size();
    double temp = (midline <= (pix.height()-midline)) ? midline : (pix.height()-midline);
    qDebug() << "temp" << temp;
    qDebug() << "h-x" << d_l_eyes->height()*pix.width()/static_cast<double>(d_l_eyes->width());

    if( (temp * d_l_eyes->width()*2) <= (pix.width()*d_l_eyes->height()) )
    {
        qDebug() << "ocular_pixmap" << 2;
        if(midline*2 <= pix.height())
        {
            qDebug() << "ocular_pixmap" << 3;
            rect.setTop(0);
            rect.setBottom(static_cast<int>(round(pix.width() * static_cast<double>(d_l_eyes->height()) / d_l_eyes->width())));
            qDebug() << "Bottom" << static_cast<int>(round(pix.width() * static_cast<double>(d_l_eyes->height()) / d_l_eyes->width()));
        }
        else
        {
            qDebug() << "ocular_pixmap" << 4;
            rect.setTop(pix.height()-static_cast<int>(round(pix.width() * static_cast<double>(d_l_eyes->height()) / d_l_eyes->width())));
            rect.setBottom(pix.height());
        }
    }
    else
    {
        qDebug() << "ocular_pixmap" << 5;
        rect.setTop(static_cast<int>(round(midline - pix.width()*d_l_eyes->height()/(2.*d_l_eyes->width()))));
        rect.setBottom(static_cast<int>(round(midline + pix.width()*d_l_eyes->height()/(2.*d_l_eyes->width()))));
    }
    qDebug() << "ocular_pixmap" << 6;
    rect.setLeft(0);
    rect.setRight(pix.width());
    pix = pix.copy(rect);
    qDebug() << "rect" << rect;
    qDebug() << "pix.size()" << pix.size();
    qDebug() << "ocular_pixmap end";
    //d_l_eyes->setPixmap(pic.scaled(d_l_eyes->size(),Qt::KeepAspectRatio));
    return pix;
}
//QPixmap MainWindow::ocular_pixmap(const Image_params &imgParams, const EyeCirclePos &left, const EyeCirclePos &right, double kscale)
//{
//    qDebug() << Q_FUNC_INFO;
//    int bytesPerLine = imgParams.frame_width;
//    qDebug() << "imgParams.frame_width" << imgParams.frame_width;
//    qDebug() << "imgParams.frame_height" << imgParams.frame_height;
//    QImage snapshot_img((uchar*)imgParams.buf.c_str(), imgParams.frame_width, imgParams.frame_height, bytesPerLine, QImage::Format_Grayscale8 /*QImage::Format_Alpha8 QImage::Format_Indexed8, QImageCleanupFunction cleanupFunction = nullptr, void *cleanupInfo = nullptr*/);
//    QPixmap pix;
//    //unsigned short midline = static_cast<unsigned short>(round((left.vert + right.vert) / 2.));
//    qDebug() << "left.horiz" << left.horiz;
//    qDebug() << "left.vert" << left.vert;
//    qDebug() << "left.radius" << left.radius;
//    qDebug() << "right.horiz" << right.horiz;
//    qDebug() << "right.vert" << right.vert;
//    qDebug() << "right.radius" << right.radius;
//    double midline = (left.vert + right.vert)*kscale / 2.;
//    qDebug() << "midline" << midline;
//    //midline = (midline <= (frame.height()-midline)) ? midline : (frame.height()-midline);
//    //qDebug() << "midline" << midline;
//    if(left.vert < left.radius
//            || right.vert < right.radius
//            || pix.height() < (left.vert + left.radius) /** kscale*/
//            || pix.height() < (right.vert + right.radius) * kscale
//            || left.horiz < left.radius
//            || pix.width() < (left.horiz + left.radius) * kscale
//            || right.horiz < right.radius
//            || pix.width() < (right.horiz + right.radius) * kscale)
//       return QPixmap();

//    //QPixmap pix(params);
//    QPainter painter;
//    painter.begin(&pix);
//    painter.setPen(QPen(QColor(STR_COLD_DARK_COLOR), 2));
//    painter.drawEllipse(QPointF(left.horiz*kscale,left.vert*kscale),left.radius*kscale,left.radius*kscale);
//    painter.drawEllipse(QPointF(right.horiz*kscale,right.vert*kscale),right.radius*kscale,right.radius*kscale);
//    if(left.horiz < right.horiz)
//        painter.drawLine(QPointF((left.horiz + left.radius)*kscale,left.vert*kscale),QPointF((right.horiz - right.radius)*kscale,right.vert*kscale));
//    else
//        painter.drawLine(QPointF((right.horiz + right.radius)*kscale,right.vert*kscale),QPointF((left.horiz - left.radius)*kscale,left.vert*kscale));
//    //painter.setPen(QPen(QColor(Qt::red), 2));
//    //painter.drawRect(pix.rect());
//    //qDebug() << "pix.rect()" << pix.rect();
//    painter.end();
//    d_l_snapshot.setPixmap(pix);
//    qDebug() << "ocular_pixmap" << 1;
//    QRect rect;

//    qDebug() << "frame.size()" << params.size();
//    qDebug() << "d_l_snapshot.size()" << d_l_snapshot.size();
//    qDebug() << "d_l_eyes.size()" << d_l_eyes->size();
//    double temp = (midline <= (params.height()-midline)) ? midline : (params.height()-midline);
//    qDebug() << "temp" << temp;
//    qDebug() << "h-x" << d_l_eyes->height()*params.width()/static_cast<double>(d_l_eyes->width());

//    if( (temp * d_l_eyes->width()*2) <= (params.width()*d_l_eyes->height()) )
//    {
//        qDebug() << "ocular_pixmap" << 2;
//        if(midline*2 <= params.height())
//        {
//            qDebug() << "ocular_pixmap" << 3;
//            rect.setTop(0);
//            rect.setBottom(static_cast<int>(round(params.width() * static_cast<double>(d_l_eyes->height()) / d_l_eyes->width())));
//            qDebug() << "Bottom" << static_cast<int>(round(params.width() * static_cast<double>(d_l_eyes->height()) / d_l_eyes->width()));
//        }
//        else
//        {
//            qDebug() << "ocular_pixmap" << 4;
//            rect.setTop(params.height()-static_cast<int>(round(params.width() * static_cast<double>(d_l_eyes->height()) / d_l_eyes->width())));
//            rect.setBottom(params.height());
//        }
//    }
//    else
//    {
//        qDebug() << "ocular_pixmap" << 5;
//        rect.setTop(static_cast<int>(round(midline - params.width()*d_l_eyes->height()/(2.*d_l_eyes->width()))));
//        rect.setBottom(static_cast<int>(round(midline + params.width()*d_l_eyes->height()/(2.*d_l_eyes->width()))));
//    }
//    qDebug() << "ocular_pixmap" << 6;
//    rect.setLeft(0);
//    rect.setRight(params.width());
//    pix = pix.copy(rect);
//    qDebug() << "rect" << rect;
//    qDebug() << "pix.size()" << pix.size();
//    qDebug() << "ocular_pixmap end";
//    //d_l_eyes->setPixmap(pic.scaled(d_l_eyes->size(),Qt::KeepAspectRatio));
//    return pix;
//}

void MainWindow::setStyle2list(const QList<QLabel *> &list, const QString &style, Qt::Alignment al, const QFont &f)
{
    qDebug() << Q_FUNC_INFO;
    foreach (QLabel * l, list) {
        if(l != nullptr)
        {
            l->setAlignment(al);
            l->setStyleSheet(style);
            l->setFont(f);
        }
    }
}
void MainWindow::setStyle2list(const QList<QLineEdit *> &list, const QPalette &p, const QString &style, Qt::Alignment al, const QFont &f)
{
    qDebug() << Q_FUNC_INFO;
    foreach (QLineEdit * l, list) {
        if(l != nullptr)
        {
            l->setAlignment(al);
            l->setStyleSheet(style);
            l->setPalette(p);
            l->setFont(f);
        }
    }
}
void MainWindow::decorateLine(QFrame * line, const QString style)
{
    qDebug() << Q_FUNC_INFO;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setStyleSheet(style);
}
//#endif
void MainWindow::resizeEvent(QResizeEvent* /*event*/)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "d_l_snapshot.size()" << d_l_snapshot.size();
    if(d_measReviewButs != nullptr){
        qDebug() << "d_measReviewButs->size()" << d_measReviewButs->size();
        d_measReviewButs->resize(d_l_snapshot.width(),d_l_snapshot.height());
        qDebug() << "d_measReviewButs->size()" << d_measReviewButs->size();
        if (d_measReviewButs_lastSize != d_measReviewButs->size())
        {
            qDebug() << "d_measReviewButs size changed from " <<d_measReviewButs_lastSize << "to " << d_measReviewButs->size();
            d_measReviewButs_lastSize = d_measReviewButs->size();
        }
    }

    //qDebug() << "d_frame_card" << d_frame_card->size();
    //qDebug() << "d_pb_shot.size()" << d_pb_shot->size();
    QSize screenSize = QGuiApplication::primaryScreen()->size();
    qDebug() << "screenSize"<<screenSize;
    qDebug() << "MainWindowSize"<<size();
//    qDebug() << "d_l_leftEye"<<d_l_leftEye->size();
//    qDebug() << "d_l_rightEye"<<d_l_rightEye->size();
//    qDebug() << "d_l_eyes"<<d_l_eyes->size();
//    qDebug() << "d_l_refractionLeft"<<d_l_refractionLeft->size();
//    qDebug() << "d_l_refractionRight"<<d_l_refractionRight->size();
//    qDebug() << "d_l_interocularRes"<<d_l_interocularRes->size();
//    qDebug() << "d_l_pic_FixLeft"<<d_l_pic_FixLeft->size();
//    qDebug() << "d_l_pic_FixRight"<<d_l_pic_FixRight->size();

    if (d_frame_card_lastSize != d_frame_card->size())
    {
        qDebug() << "d_frame_card size changed from " <<d_frame_card_lastSize << "to " << d_frame_card->size();
        d_frame_card_lastSize = d_frame_card->size();
    }
    if (frame_data_total_lastSize != frame_data_total->size())
    {
        qDebug() << "frame_data_total size changed from " <<frame_data_total_lastSize << "to " << frame_data_total->size();
        frame_data_total_lastSize = frame_data_total->size();
    }
    if (frame_dataResults_total_lastSize != frame_dataResults_total->size())
    {
        qDebug() << "frame_dataResults_total size changed from " <<frame_dataResults_total_lastSize << "to " << frame_dataResults_total->size();
        frame_dataResults_total_lastSize = frame_dataResults_total->size();
    }

    if (d_pb_shot_lastSize != d_pb_shot->size())
    {
        qDebug() << "d_pb_shot size changed from " <<d_pb_shot_lastSize << "to " << d_pb_shot->size();
        d_pb_shot_lastSize = d_pb_shot->size();
    }
    if (d_l_snapshot_lastSize != d_l_snapshot.size())
    {
        qDebug() << "d_l_snapshot size changed from " <<d_l_snapshot_lastSize << "to " << d_l_snapshot.size();
        d_l_snapshot_lastSize = d_l_snapshot.size();
    }
    if (d_l_leftEye_lastSize != d_l_leftEye->size())
    {
        qDebug() << "d_l_leftEye size changed from " <<d_l_leftEye_lastSize << "to " << d_l_leftEye->size();
        d_l_leftEye_lastSize = d_l_leftEye->size();
    }
    if (d_l_rightEye_lastSize != d_l_rightEye->size())
    {
        qDebug() << "d_l_rightEye size changed from " <<d_l_rightEye_lastSize << "to " << d_l_rightEye->size();
        d_l_rightEye_lastSize = d_l_rightEye->size();
    }
    if (d_l_eyes_lastSize != d_l_eyes->size())
    {
        qDebug() << "d_l_eyes size changed from " <<d_l_eyes_lastSize << "to " << d_l_eyes->size();
        d_l_eyes_lastSize = d_l_eyes->size();
    }
    if (d_l_refractionLeft_lastSize != d_l_refractionLeft->size())
    {
        qDebug() << "d_l_refractionLeft size changed from " <<d_l_refractionLeft_lastSize << "to " << d_l_refractionLeft->size();
        d_l_refractionLeft_lastSize = d_l_refractionLeft->size();
    }
    if (d_l_refractionRight_lastSize != d_l_refractionRight->size())
    {
        qDebug() << "d_l_refractionRight size changed from " <<d_l_refractionRight_lastSize << "to " << d_l_refractionRight->size();
        d_l_refractionRight_lastSize = d_l_refractionRight->size();
    }
    if (d_l_interocularRes_lastSize != d_l_interocularRes->size())
    {
        qDebug() << "d_l_interocularRes size changed from " <<d_l_interocularRes_lastSize << "to " << d_l_interocularRes->size();
        d_l_interocularRes_lastSize = d_l_interocularRes->size();
    }
    if (d_l_pic_FixLeft_lastSize != d_l_pic_FixLeft->size())
    {
        qDebug() << "d_l_pic_FixLeft size changed from " <<d_l_pic_FixLeft_lastSize << "to " << d_l_pic_FixLeft->size();
        d_l_pic_FixLeft_lastSize = d_l_pic_FixLeft->size();
    }
    if (d_l_pic_FixRight_lastSize != d_l_pic_FixRight->size())
    {
        qDebug() << "d_l_pic_FixRight size changed from " <<d_l_pic_FixRight_lastSize << "to " << d_l_pic_FixRight->size();
        d_l_pic_FixRight_lastSize = d_l_pic_FixRight->size();
    }
}
void MainWindow::showEvent(QShowEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    QSize screenSize = QGuiApplication::primaryScreen()->size();
    qDebug() << "screenSize"<<screenSize;
    qDebug() << "MainWindowSize"<<size();
    qDebug() << "d_l_snapshot.size()" << d_l_snapshot.size();
    qDebug() << "d_frame_card" << d_frame_card->size();
    qDebug() << "d_pb_shot.size()" << d_pb_shot->size();
    qDebug() << "d_l_leftEye"<<d_l_leftEye->size();
    qDebug() << "d_l_rightEye"<<d_l_rightEye->size();
    qDebug() << "d_l_eyes"<<d_l_eyes->size();
    qDebug() << "d_l_refractionLeft"<<d_l_refractionLeft->size();
    qDebug() << "d_l_refractionRight"<<d_l_refractionRight->size();
    qDebug() << "d_l_interocularRes"<<d_l_interocularRes->size();
    qDebug() << "d_l_pic_FixLeft"<<d_l_pic_FixLeft->size();
    qDebug() << "d_l_pic_FixRight"<<d_l_pic_FixRight->size();
    if (d_frame_card_lastSize != d_frame_card->size())
    {
        qDebug() << "d_frame_card size changed from " <<d_frame_card_lastSize << "to " << d_frame_card->size();
        d_frame_card_lastSize = d_frame_card->size();
    }
    if (d_pb_shot_lastSize != d_pb_shot->size())
    {
        qDebug() << "d_pb_shot size changed from " <<d_pb_shot_lastSize << "to " << d_pb_shot->size();
        d_pb_shot_lastSize = d_pb_shot->size();
    }
    if (d_l_snapshot_lastSize != d_l_snapshot.size())
    {
        qDebug() << "d_l_snapshot size changed from " <<d_l_snapshot_lastSize << "to " << d_l_snapshot.size();
        d_l_snapshot_lastSize = d_l_snapshot.size();
    }
    if (d_l_leftEye_lastSize != d_l_leftEye->size())
    {
        qDebug() << "d_l_leftEye size changed from " <<d_l_leftEye_lastSize << "to " << d_l_leftEye->size();
        d_l_leftEye_lastSize = d_l_leftEye->size();
    }
    if (d_l_rightEye_lastSize != d_l_rightEye->size())
    {
        qDebug() << "d_l_rightEye size changed from " <<d_l_rightEye_lastSize << "to " << d_l_rightEye->size();
        d_l_rightEye_lastSize = d_l_rightEye->size();
    }
    if (d_l_eyes_lastSize != d_l_eyes->size())
    {
        qDebug() << "d_l_eyes size changed from " <<d_l_eyes_lastSize << "to " << d_l_eyes->size();
        d_l_eyes_lastSize = d_l_eyes->size();
    }
    if (d_l_refractionLeft_lastSize != d_l_refractionLeft->size())
    {
        qDebug() << "d_l_refractionLeft size changed from " <<d_l_refractionLeft_lastSize << "to " << d_l_refractionLeft->size();
        d_l_refractionLeft_lastSize = d_l_refractionLeft->size();
    }
    if (d_l_refractionRight_lastSize != d_l_refractionRight->size())
    {
        qDebug() << "d_l_refractionRight size changed from " <<d_l_refractionRight_lastSize << "to " << d_l_refractionRight->size();
        d_l_refractionRight_lastSize = d_l_refractionRight->size();
    }
    if (d_l_interocularRes_lastSize != d_l_interocularRes->size())
    {
        qDebug() << "d_l_interocularRes size changed from " <<d_l_interocularRes_lastSize << "to " << d_l_interocularRes->size();
        d_l_interocularRes_lastSize = d_l_interocularRes->size();
    }
    if (d_l_pic_FixLeft_lastSize != d_l_pic_FixLeft->size())
    {
        qDebug() << "d_l_pic_FixLeft size changed from " <<d_l_pic_FixLeft_lastSize << "to " << d_l_pic_FixLeft->size();
        d_l_pic_FixLeft_lastSize = d_l_pic_FixLeft->size();
    }
    if (d_l_pic_FixRight_lastSize != d_l_pic_FixRight->size())
    {
        qDebug() << "d_l_pic_FixRight size changed from " <<d_l_pic_FixRight_lastSize << "to " << d_l_pic_FixRight->size();
        d_l_pic_FixRight_lastSize = d_l_pic_FixRight->size();
    }
//    d_frame_card->setFixedSize( d_frame_card->size());
//    d_pb_shot->setFixedSize(d_pb_shot->size());
//    d_l_leftEye->setFixedSize(d_l_leftEye->size());
//    d_l_rightEye->setFixedSize(d_l_rightEye->size());
//    d_l_eyes->setFixedSize(d_l_eyes->size());
//    d_l_refractionLeft->setFixedSize(d_l_refractionLeft->size());
//    d_l_refractionRight->setFixedSize(d_l_refractionRight->size());
//    d_l_interocularRes->setFixedSize(d_l_interocularRes->size());
//    d_l_pic_FixLeft->setFixedSize(d_l_pic_FixLeft->size());
//    d_l_pic_FixRight->setFixedSize(d_l_pic_FixRight->size());
    return QMainWindow::showEvent(event);
}
void MainWindow::paintEvent(QPaintEvent *event)
{
    qDebug() << Q_FUNC_INFO;
    QSize screenSize = QGuiApplication::primaryScreen()->size();
    qDebug() << "screenSize"<<screenSize;
    qDebug() << "MainWindowSize"<<size();
    qDebug() << "d_l_snapshot.size()" << d_l_snapshot.size();
    qDebug() << "d_frame_card" << d_frame_card->size();
    qDebug() << "d_pb_shot.size()" << d_pb_shot->size();
    qDebug() << "d_l_leftEye"<<d_l_leftEye->size();
    qDebug() << "d_l_rightEye"<<d_l_rightEye->size();
    qDebug() << "d_l_eyes"<<d_l_eyes->size();
    qDebug() << "d_l_refractionLeft"<<d_l_refractionLeft->size();
    qDebug() << "d_l_refractionRight"<<d_l_refractionRight->size();
    qDebug() << "d_l_interocularRes"<<d_l_interocularRes->size();
    qDebug() << "d_l_pic_FixLeft"<<d_l_pic_FixLeft->size();
    qDebug() << "d_l_pic_FixRight"<<d_l_pic_FixRight->size();
    if (d_frame_card_lastSize != d_frame_card->size())
    {
        qDebug() << "d_frame_card size changed from " <<d_frame_card_lastSize << "to " << d_frame_card->size();
        d_frame_card_lastSize = d_frame_card->size();
    }
    if (d_pb_shot_lastSize != d_pb_shot->size())
    {
        qDebug() << "d_pb_shot size changed from " <<d_pb_shot_lastSize << "to " << d_pb_shot->size();
        d_pb_shot_lastSize = d_pb_shot->size();
    }
    if (d_l_snapshot_lastSize != d_l_snapshot.size())
    {
        qDebug() << "d_l_snapshot size changed from " <<d_l_snapshot_lastSize << "to " << d_l_snapshot.size();
        d_l_snapshot_lastSize = d_l_snapshot.size();
    }
    if (d_l_leftEye_lastSize != d_l_leftEye->size())
    {
        qDebug() << "d_l_leftEye size changed from " <<d_l_leftEye_lastSize << "to " << d_l_leftEye->size();
        d_l_leftEye_lastSize = d_l_leftEye->size();
    }
    if (d_l_rightEye_lastSize != d_l_rightEye->size())
    {
        qDebug() << "d_l_rightEye size changed from " <<d_l_rightEye_lastSize << "to " << d_l_rightEye->size();
        d_l_rightEye_lastSize = d_l_rightEye->size();
    }
    if (d_l_eyes_lastSize != d_l_eyes->size())
    {
        qDebug() << "d_l_eyes size changed from " <<d_l_eyes_lastSize << "to " << d_l_eyes->size();
        d_l_eyes_lastSize = d_l_eyes->size();
    }
    if (d_l_refractionLeft_lastSize != d_l_refractionLeft->size())
    {
        qDebug() << "d_l_refractionLeft size changed from " <<d_l_refractionLeft_lastSize << "to " << d_l_refractionLeft->size();
        d_l_refractionLeft_lastSize = d_l_refractionLeft->size();
    }
    if (d_l_refractionRight_lastSize != d_l_refractionRight->size())
    {
        qDebug() << "d_l_refractionRight size changed from " <<d_l_refractionRight_lastSize << "to " << d_l_refractionRight->size();
        d_l_refractionRight_lastSize = d_l_refractionRight->size();
    }
    if (d_l_interocularRes_lastSize != d_l_interocularRes->size())
    {
        qDebug() << "d_l_interocularRes size changed from " <<d_l_interocularRes_lastSize << "to " << d_l_interocularRes->size();
        d_l_interocularRes_lastSize = d_l_interocularRes->size();
    }
    if (d_l_pic_FixLeft_lastSize != d_l_pic_FixLeft->size())
    {
        qDebug() << "d_l_pic_FixLeft size changed from " <<d_l_pic_FixLeft_lastSize << "to " << d_l_pic_FixLeft->size();
        d_l_pic_FixLeft_lastSize = d_l_pic_FixLeft->size();
    }
    if (d_l_pic_FixRight_lastSize != d_l_pic_FixRight->size())
    {
        qDebug() << "d_l_pic_FixRight size changed from " <<d_l_pic_FixRight_lastSize << "to " << d_l_pic_FixRight->size();
        d_l_pic_FixRight_lastSize = d_l_pic_FixRight->size();
    }
//    d_frame_card->setFixedSize( d_frame_card->size());
//    d_pb_shot->setFixedSize(d_pb_shot->size());
//    d_l_leftEye->setFixedSize(d_l_leftEye->size());
//    d_l_rightEye->setFixedSize(d_l_rightEye->size());
//    d_l_eyes->setFixedSize(d_l_eyes->size());
//    d_l_refractionLeft->setFixedSize(d_l_refractionLeft->size());
//    d_l_refractionRight->setFixedSize(d_l_refractionRight->size());
//    d_l_interocularRes->setFixedSize(d_l_interocularRes->size());
//    d_l_pic_FixLeft->setFixedSize(d_l_pic_FixLeft->size());
//    d_l_pic_FixRight->setFixedSize(d_l_pic_FixRight->size());
    return QMainWindow::paintEvent(event);
}
//bool MainWindow::event(QEvent* e) {
//    qDebug() << Q_FUNC_INFO;
//    const auto keyboard_rect = QGuiApplication::inputMethod()->keyboardRectangle();
//    const auto keyboard_visible = QGuiApplication::inputMethod()->isVisible();
//    const auto global_y = QWidget::mapToGlobal(rect().topLeft()).y() + height();
//    const auto k_global_y = keyboard_rect.topLeft().y();
//    const auto diff = k_global_y - global_y;
//    const auto need_to_move = diff < 0;


//    /* move main widget */
//    if (keyboard_visible /*&& !_moved && need_to_move*/) {
//        qDebug() << Q_FUNC_INFO << "move main widget";
//        //_moved = true;
//        //_lastDiff = diff;
//        //const auto g = parentWidget()->frameGeometry();
//        //parentWidget()->move(g.x(), g.y() - qAbs(_lastDiff));
//        //d_l_duplicate->show();
//        int screenHeight = QGuiApplication::primaryScreen()->size().height();
//        QRectF vb_rect = QGuiApplication::inputMethod()->inputItemRectangle();
//        qDebug() << vb_rect;
//        //QRectF vb_rectNew(static_cast<qreal>(0.), static_cast<qreal>(screenHeight*2/3.), static_cast<qreal>(QGuiApplication::primaryScreen()->size().width()) , static_cast<qreal>(screenHeight/3.));
//        vb_rect.setBottomLeft(QPointF(0, screenHeight*2/3.));
//        //vb_rect.setHeight(screenHeight/3.);
//        //qDebug() << vb_rectNew;
//        qDebug() << QGuiApplication::inputMethod()->inputItemRectangle();
//        QGuiApplication::inputMethod()->setInputItemRectangle(vb_rect);
//        qDebug() << QGuiApplication::inputMethod()->inputItemRectangle();
//        QTransform trans = QGuiApplication::inputMethod()->inputItemTransform();
//        trans.translate(100,100);
//        trans.scale(0.5,1.0);
//        QGuiApplication::inputMethod()->setInputItemTransform(trans);
//    }
//    /* roll back */
////    if (!keyboard_visible && _moved) {
////        qDebug() << Q_FUNC_INFO << "roll back";
////        _moved = false;
////        const auto g = parentWidget()->frameGeometry();
////        parentWidget()->move(g.x(), g.y() + qAbs(_lastDiff));
////        //d_le_duplicate->hide();
////    }
//    return QMainWindow::event(e);
//}
