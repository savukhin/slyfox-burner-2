/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QWidget *options;
    QVBoxLayout *verticalLayout_3;
    QScrollArea *scrollArea;
    QWidget *optionsScrollareaContents;
    QVBoxLayout *verticalLayout_5;
    QWidget *step_group;
    QGridLayout *gridLayout_3;
    QLabel *label_11;
    QLabel *label_12;
    QLineEdit *stepLineEdit;
    QWidget *directional_buttons_group;
    QGridLayout *gridLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_4;
    QPushButton *pushButton_3;
    QFrame *line;
    QWidget *speeds_group;
    QGridLayout *gridLayout_2;
    QLineEdit *rapid_speed_input;
    QLineEdit *lineEdit_5;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label;
    QLabel *label_4;
    QLineEdit *lineEdit_3;
    QLabel *label_9;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_7;
    QLabel *rapid_speed_label;
    QLabel *rapid_speed_mms_label;
    QLineEdit *lineEdit_2;
    QLineEdit *lineEdit_4;
    QLabel *label_10;
    QLabel *label_8;
    QPushButton *pushButton_5;
    QSpacerItem *verticalSpacer;
    QWidget *content;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1006, 635);
        MainWindow->setMinimumSize(QSize(0, 0));
        QPalette palette;
        MainWindow->setPalette(palette);
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setMinimumSize(QSize(0, 0));
        centralwidget->setStyleSheet(QString::fromUtf8(".QWidget {\n"
"	border: 2px solid green;\n"
"	border-radius: 10px;\n"
"}\n"
"\n"
"#centralwidget {\n"
"	border: 2px solid blue;\n"
"	border-radius: 10px;\n"
"}"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetMaximumSize);
        options = new QWidget(centralwidget);
        options->setObjectName(QString::fromUtf8("options"));
        options->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(options->sizePolicy().hasHeightForWidth());
        options->setSizePolicy(sizePolicy);
        options->setMinimumSize(QSize(0, 0));
        options->setAutoFillBackground(false);
        options->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_3 = new QVBoxLayout(options);
        verticalLayout_3->setSpacing(8);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetMinimumSize);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(options);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy1);
        scrollArea->setMinimumSize(QSize(350, 500));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        scrollArea->setWidgetResizable(true);
        optionsScrollareaContents = new QWidget();
        optionsScrollareaContents->setObjectName(QString::fromUtf8("optionsScrollareaContents"));
        optionsScrollareaContents->setGeometry(QRect(0, 0, 348, 572));
        optionsScrollareaContents->setStyleSheet(QString::fromUtf8("#optionsScrollareaContents > .QWidget {\n"
"	border: 2px solid red;\n"
"	border: none;\n"
"	border-radius: 0;\n"
"}\n"
"\n"
"#optionsScrollareaContents > Line {\n"
"	border: 10px solid black;\n"
"}"));
        verticalLayout_5 = new QVBoxLayout(optionsScrollareaContents);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        step_group = new QWidget(optionsScrollareaContents);
        step_group->setObjectName(QString::fromUtf8("step_group"));
        gridLayout_3 = new QGridLayout(step_group);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_11 = new QLabel(step_group);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout_3->addWidget(label_11, 0, 0, 1, 1);

        label_12 = new QLabel(step_group);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout_3->addWidget(label_12, 0, 2, 1, 1);

        stepLineEdit = new QLineEdit(step_group);
        stepLineEdit->setObjectName(QString::fromUtf8("stepLineEdit"));
        stepLineEdit->setClearButtonEnabled(false);

        gridLayout_3->addWidget(stepLineEdit, 0, 1, 1, 1);


        verticalLayout_5->addWidget(step_group);

        directional_buttons_group = new QWidget(optionsScrollareaContents);
        directional_buttons_group->setObjectName(QString::fromUtf8("directional_buttons_group"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(directional_buttons_group->sizePolicy().hasHeightForWidth());
        directional_buttons_group->setSizePolicy(sizePolicy2);
        directional_buttons_group->setLayoutDirection(Qt::LeftToRight);
        directional_buttons_group->setAutoFillBackground(false);
        directional_buttons_group->setStyleSheet(QString::fromUtf8("\n"
"QPushButton {\n"
"	height: 60px;\n"
"	width: 60px;\n"
"}"));
        gridLayout = new QGridLayout(directional_buttons_group);
        gridLayout->setSpacing(9);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetFixedSize);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        pushButton = new QPushButton(directional_buttons_group);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy3);
        pushButton->setCheckable(false);

        gridLayout->addWidget(pushButton, 0, 1, 1, 1);

        pushButton_2 = new QPushButton(directional_buttons_group);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        sizePolicy3.setHeightForWidth(pushButton_2->sizePolicy().hasHeightForWidth());
        pushButton_2->setSizePolicy(sizePolicy3);

        gridLayout->addWidget(pushButton_2, 1, 1, 1, 1);

        pushButton_4 = new QPushButton(directional_buttons_group);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        sizePolicy3.setHeightForWidth(pushButton_4->sizePolicy().hasHeightForWidth());
        pushButton_4->setSizePolicy(sizePolicy3);

        gridLayout->addWidget(pushButton_4, 1, 0, 1, 1);

        pushButton_3 = new QPushButton(directional_buttons_group);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        sizePolicy3.setHeightForWidth(pushButton_3->sizePolicy().hasHeightForWidth());
        pushButton_3->setSizePolicy(sizePolicy3);

        gridLayout->addWidget(pushButton_3, 1, 2, 1, 1);


        verticalLayout_5->addWidget(directional_buttons_group, 0, Qt::AlignHCenter);

        line = new QFrame(optionsScrollareaContents);
        line->setObjectName(QString::fromUtf8("line"));
        line->setStyleSheet(QString::fromUtf8(""));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_5->addWidget(line);

        speeds_group = new QWidget(optionsScrollareaContents);
        speeds_group->setObjectName(QString::fromUtf8("speeds_group"));
        sizePolicy2.setHeightForWidth(speeds_group->sizePolicy().hasHeightForWidth());
        speeds_group->setSizePolicy(sizePolicy2);
        gridLayout_2 = new QGridLayout(speeds_group);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setSizeConstraint(QLayout::SetFixedSize);
        gridLayout_2->setContentsMargins(-1, -1, 1, -1);
        rapid_speed_input = new QLineEdit(speeds_group);
        rapid_speed_input->setObjectName(QString::fromUtf8("rapid_speed_input"));

        gridLayout_2->addWidget(rapid_speed_input, 0, 2, 1, 1);

        lineEdit_5 = new QLineEdit(speeds_group);
        lineEdit_5->setObjectName(QString::fromUtf8("lineEdit_5"));

        gridLayout_2->addWidget(lineEdit_5, 5, 2, 1, 1);

        label_5 = new QLabel(speeds_group);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout_2->addWidget(label_5, 5, 1, 1, 1);

        label_6 = new QLabel(speeds_group);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_2->addWidget(label_6, 1, 3, 1, 1);

        label = new QLabel(speeds_group);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_2->addWidget(label, 1, 1, 1, 1);

        label_4 = new QLabel(speeds_group);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 4, 1, 1, 1);

        lineEdit_3 = new QLineEdit(speeds_group);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));

        gridLayout_2->addWidget(lineEdit_3, 3, 2, 1, 1);

        label_9 = new QLabel(speeds_group);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_2->addWidget(label_9, 4, 3, 1, 1);

        lineEdit = new QLineEdit(speeds_group);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        gridLayout_2->addWidget(lineEdit, 1, 2, 1, 1);

        label_2 = new QLabel(speeds_group);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_2->addWidget(label_2, 2, 1, 1, 1);

        label_3 = new QLabel(speeds_group);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 3, 1, 1, 1);

        label_7 = new QLabel(speeds_group);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_2->addWidget(label_7, 2, 3, 1, 1);

        rapid_speed_label = new QLabel(speeds_group);
        rapid_speed_label->setObjectName(QString::fromUtf8("rapid_speed_label"));

        gridLayout_2->addWidget(rapid_speed_label, 0, 1, 1, 1);

        rapid_speed_mms_label = new QLabel(speeds_group);
        rapid_speed_mms_label->setObjectName(QString::fromUtf8("rapid_speed_mms_label"));

        gridLayout_2->addWidget(rapid_speed_mms_label, 0, 3, 1, 1);

        lineEdit_2 = new QLineEdit(speeds_group);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        gridLayout_2->addWidget(lineEdit_2, 2, 2, 1, 1);

        lineEdit_4 = new QLineEdit(speeds_group);
        lineEdit_4->setObjectName(QString::fromUtf8("lineEdit_4"));

        gridLayout_2->addWidget(lineEdit_4, 4, 2, 1, 1);

        label_10 = new QLabel(speeds_group);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_2->addWidget(label_10, 5, 3, 1, 1);

        label_8 = new QLabel(speeds_group);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_2->addWidget(label_8, 3, 3, 1, 1);


        verticalLayout_5->addWidget(speeds_group);

        pushButton_5 = new QPushButton(optionsScrollareaContents);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));

        verticalLayout_5->addWidget(pushButton_5);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer);

        scrollArea->setWidget(optionsScrollareaContents);

        verticalLayout_3->addWidget(scrollArea);


        horizontalLayout->addWidget(options);

        content = new QWidget(centralwidget);
        content->setObjectName(QString::fromUtf8("content"));
        content->setMinimumSize(QSize(0, 10));
        content->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_2 = new QVBoxLayout(content);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

        verticalLayout_2->addLayout(verticalLayout);


        horizontalLayout->addWidget(content);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1006, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "Step ", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "mm", nullptr));
        stepLineEdit->setInputMask(QString());
        stepLineEdit->setText(QString());
        pushButton->setText(QCoreApplication::translate("MainWindow", "\342\206\221", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "\342\206\223", nullptr));
        pushButton_4->setText(QCoreApplication::translate("MainWindow", "\342\206\220", nullptr));
        pushButton_3->setText(QCoreApplication::translate("MainWindow", "\342\206\222", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Accel Y", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "mm/s", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Rapid Y Speed", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Accel X", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "mm/s^2", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Low X Speed", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Low Y Speed", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "mm/s", nullptr));
        rapid_speed_label->setText(QCoreApplication::translate("MainWindow", "Rapid X Speed", nullptr));
        rapid_speed_mms_label->setText(QCoreApplication::translate("MainWindow", "mm/s", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "mm/s^2", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "mm/s", nullptr));
        pushButton_5->setText(QCoreApplication::translate("MainWindow", "Start Experiment", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
