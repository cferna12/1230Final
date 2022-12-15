#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "realtime.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void initialize();
    void finish();

private:
    void connectUIElements();
    void connectParam1();
    void connectParam2();
    void connectNear();
    void connectFar();
    void connectPerPixelFilter();
    void connectKernelBasedFilter();
    void connectUploadFile();
    void connectExtraCredit();
    void connectToon();
    void connectToon2();
    void connectToon3();
    void connectToon4();


    Realtime *realtime;
    QCheckBox *filter1;
    QCheckBox *filter2;
    QPushButton *uploadFile;
    QSlider *p1Slider;
    QSlider *p2Slider;
    QSpinBox *p1Box;
    QSpinBox *p2Box;
    QSlider *nearSlider;
    QSlider *farSlider;
    QDoubleSpinBox *nearBox;
    QDoubleSpinBox *farBox;

    QSlider *toon1Slider;
    QSpinBox *toon1Box;

    QSlider *toon2Slider;
    QSpinBox *toon2Box;

    QSlider *toon3Slider;
    QDoubleSpinBox *toon3Box;

    QSlider *toon4Slider;
    QDoubleSpinBox *toon4Box;
    // Extra Credit:
    QCheckBox *ec1;
    QCheckBox *ec2;
    QCheckBox *ec3;
    QCheckBox *ec4;

private slots:
    void onPerPixelFilter();
    void onKernelBasedFilter();
    void onUploadFile();
    void onValChangeP1(int newValue);
    void onValChangeP2(int newValue);
    void onValChangeNearSlider(int newValue);
    void onValChangeFarSlider(int newValue);
    void onValChangeNearBox(double newValue);
    void onValChangeFarBox(double newValue);

    void onValChangeToon1(int newValue);
    void onValChangeToon2(int newValue);
    void onValChangeToon3Slider(int newValue);
    void onValChangeToon3Box(double newValue);

    void onValChangeToon4Slider(int newValue);
    void onValChangeToon4Box(double newValue);
    // Extra Credit:
    void onExtraCredit1();
    void onExtraCredit2();
    void onExtraCredit3();
    void onExtraCredit4();
};
