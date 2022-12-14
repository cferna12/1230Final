#include "mainwindow.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <iostream>

void MainWindow::initialize() {
    realtime = new Realtime;

    QHBoxLayout *hLayout = new QHBoxLayout; // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(realtime, 1);
    this->setLayout(hLayout);

    // Create labels in sidebox
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
//    QLabel *tesselation_label = new QLabel(); // Parameters label
//    tesselation_label->setText("Tesselation");
//    tesselation_label->setFont(font);
    QLabel *camera_label = new QLabel(); // Camera label
    camera_label->setText("Camera");
    camera_label->setFont(font);
//    QLabel *filters_label = new QLabel(); // Filters label
//    filters_label->setText("Filters");
//    filters_label->setFont(font);
    QLabel *ec_label = new QLabel(); // Extra Credit label
    ec_label->setText("Extra Credit");
    ec_label->setFont(font);
//    QLabel *param1_label = new QLabel(); // Parameter 1 label
//    param1_label->setText("Parameter 1:");
//    QLabel *param2_label = new QLabel(); // Parameter 2 label
//    param2_label->setText("Parameter 2:");
    QLabel *near_label = new QLabel(); // Near plane label
    near_label->setText("Near Plane:");
    QLabel *far_label = new QLabel(); // Far plane label
    far_label->setText("Far Plane:");

    QLabel *toon_label = new QLabel(); // Extra Credit label
    toon_label->setText("Toon Shading");
    toon_label->setFont(font);



//    // Create checkbox for per-pixel filter
//    filter1 = new QCheckBox();
//    filter1->setText(QStringLiteral("Per-Pixel Filter"));
//    filter1->setChecked(false);

//    // Create checkbox for kernel-based filter
//    filter2 = new QCheckBox();
//    filter2->setText(QStringLiteral("Kernel-Based Filter"));
//    filter2->setChecked(false);

    // Create file uploader for scene file
    uploadFile = new QPushButton();
    uploadFile->setText(QStringLiteral("Upload Scene File"));

//    // Creates the boxes containing the parameter sliders and number boxes
//    QGroupBox *p1Layout = new QGroupBox(); // horizonal slider 1 alignment
//    QHBoxLayout *l1 = new QHBoxLayout();
//    QGroupBox *p2Layout = new QGroupBox(); // horizonal slider 2 alignment
//    QHBoxLayout *l2 = new QHBoxLayout();

//    // Create slider controls to control parameters
//    p1Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
//    p1Slider->setTickInterval(1);
//    p1Slider->setMinimum(1);
//    p1Slider->setMaximum(25);
//    p1Slider->setValue(1);

//    p1Box = new QSpinBox();
//    p1Box->setMinimum(1);
//    p1Box->setMaximum(25);
//    p1Box->setSingleStep(1);
//    p1Box->setValue(1);

//    p2Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 2 slider
//    p2Slider->setTickInterval(1);
//    p2Slider->setMinimum(1);
//    p2Slider->setMaximum(25);
//    p2Slider->setValue(1);

//    p2Box = new QSpinBox();
//    p2Box->setMinimum(1);
//    p2Box->setMaximum(25);
//    p2Box->setSingleStep(1);
//    p2Box->setValue(1);

//    // Adds the slider and number box to the parameter layouts
//    l1->addWidget(p1Slider);
//    l1->addWidget(p1Box);
//    p1Layout->setLayout(l1);

//    l2->addWidget(p2Slider);
//    l2->addWidget(p2Box);
//    p2Layout->setLayout(l2);

    // Creates the boxes containing the camera sliders and number boxes
    QGroupBox *nearLayout = new QGroupBox(); // horizonal near slider alignment
    QHBoxLayout *lnear = new QHBoxLayout();
    QGroupBox *farLayout = new QGroupBox(); // horizonal far slider alignment
    QHBoxLayout *lfar = new QHBoxLayout();

    // Create slider controls to control near/far planes
    nearSlider = new QSlider(Qt::Orientation::Horizontal); // Near plane slider
    nearSlider->setTickInterval(1);
    nearSlider->setMinimum(1);
    nearSlider->setMaximum(1000);
    nearSlider->setValue(10);

    nearBox = new QDoubleSpinBox();
    nearBox->setMinimum(0.01f);
    nearBox->setMaximum(10.f);
    nearBox->setSingleStep(0.1f);
    nearBox->setValue(0.1f);

    farSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
    farSlider->setTickInterval(1);
    farSlider->setMinimum(1000);
    farSlider->setMaximum(10000);
    farSlider->setValue(10000);

    farBox = new QDoubleSpinBox();
    farBox->setMinimum(10.f);
    farBox->setMaximum(100.f);
    farBox->setSingleStep(0.1f);
    farBox->setValue(100.f);

    // Adds the slider and number box to the parameter layouts
    lnear->addWidget(nearSlider);
    lnear->addWidget(nearBox);
    nearLayout->setLayout(lnear);

    lfar->addWidget(farSlider);
    lfar->addWidget(farBox);
    farLayout->setLayout(lfar);

    /*Below is for toon shading*/
    QGroupBox *toon3Layout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *layout3 = new QHBoxLayout();
    toon3Slider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
    toon3Slider->setTickInterval(1);
    toon3Slider->setMinimum(0);
    toon3Slider->setMaximum(50);
    toon3Slider->setValue(0);

    toon3Box = new QDoubleSpinBox();
    toon3Box->setMinimum(0.f);
    toon3Box->setMaximum(5.f);
    toon3Box->setSingleStep(0.1f);
    toon3Box->setValue(0.f);
    layout3->addWidget(toon3Slider);
    layout3->addWidget(toon3Box);
    toon3Layout->setLayout(layout3);


    QGroupBox *toon1Layout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *layout1 = new QHBoxLayout();

    toon1Slider = new QSlider(Qt::Orientation::Horizontal);
    toon1Slider->setTickInterval(1);
    toon1Slider->setMinimum(0);
    toon1Slider->setMaximum(5);
    toon1Slider->setValue(0);

    toon1Box = new QSpinBox();
    toon1Box->setMinimum(0);
    toon1Box->setMaximum(5);
    toon1Box->setSingleStep(1);
    toon1Box->setValue(0);

    layout1->addWidget(toon1Slider);
    layout1->addWidget(toon1Box);
    toon1Layout->setLayout(layout1);



    QGroupBox *toon2Layout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *layout2 = new QHBoxLayout();

    toon2Slider = new QSlider(Qt::Orientation::Horizontal);
    toon2Slider->setTickInterval(1);
    toon2Slider->setMinimum(1);
    toon2Slider->setMaximum(5);
    toon2Slider->setValue(1);

    toon2Box = new QSpinBox();
    toon2Box->setMinimum(1);
    toon2Box->setMaximum(5);
    toon2Box->setSingleStep(1);
    toon2Box->setValue(1);

    layout2->addWidget(toon2Slider);
    layout2->addWidget(toon2Box);
    toon2Layout->setLayout(layout2);
    // Extra Credit:
    ec1 = new QCheckBox();
    ec1->setText(QStringLiteral("Extra Credit 1"));
    ec1->setChecked(false);

    ec2 = new QCheckBox();
    ec2->setText(QStringLiteral("Extra Credit 2"));
    ec2->setChecked(false);

    ec3 = new QCheckBox();
    ec3->setText(QStringLiteral("Extra Credit 3"));
    ec3->setChecked(false);

    ec4 = new QCheckBox();
    ec4->setText(QStringLiteral("Extra Credit 4"));
    ec4->setChecked(false);

    vLayout->addWidget(uploadFile);
//    vLayout->addWidget(tesselation_label);
//    vLayout->addWidget(param1_label);
//    vLayout->addWidget(p1Layout);
//    vLayout->addWidget(param2_label);
//    vLayout->addWidget(p2Layout);
    vLayout->addWidget(camera_label);
    vLayout->addWidget(near_label);
    vLayout->addWidget(nearLayout);
    vLayout->addWidget(far_label);
    vLayout->addWidget(farLayout);
//    vLayout->addWidget(filters_label);
//    vLayout->addWidget(filter1);
//    vLayout->addWidget(filter2);
    // Extra Credit:
    vLayout->addWidget(ec_label);
    vLayout->addWidget(ec1);
    vLayout->addWidget(ec2);
    vLayout->addWidget(ec3);
    vLayout->addWidget(ec4);
    vLayout->addWidget(toon_label);
    vLayout->addWidget(toon1Layout);
    vLayout->addWidget(toon2Layout);
    vLayout->addWidget(toon3Layout);
    connectUIElements();

    // Set default values of 5 for tesselation parameters
//    onValChangeP1(25);
//    onValChangeP2(25);

    // Set default values for near and far planes
    onValChangeNearBox(0.1f);
    onValChangeFarBox(10.f);

    onValChangeToon1(0);

    onValChangeToon2(1);
    onValChangeToon3Box(0.f);
}

void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {
//    connectPerPixelFilter();
//    connectKernelBasedFilter();
    connectUploadFile();
//    connectParam1();
//    connectParam2();
    connectNear();
    connectFar();
    connectExtraCredit();
    connectToon();
    connectToon2();
    connectToon3();
}


void MainWindow::connectToon() {
    connect(toon1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeToon1);
    connect(toon1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeToon1);
}

void MainWindow::connectToon2() {
    connect(toon2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeToon2);
    connect(toon2Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeToon2);
}

void MainWindow::connectToon3() {
    connect(toon3Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeToon3Slider);
    connect(toon3Box, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeToon3Box);
}
void MainWindow::connectPerPixelFilter() {
    connect(filter1, &QCheckBox::clicked, this, &MainWindow::onPerPixelFilter);
}

void MainWindow::connectKernelBasedFilter() {
    connect(filter2, &QCheckBox::clicked, this, &MainWindow::onKernelBasedFilter);
}

void MainWindow::connectUploadFile() {
    connect(uploadFile, &QPushButton::clicked, this, &MainWindow::onUploadFile);
}

void MainWindow::connectParam1() {
    connect(p1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP1);
    connect(p1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP1);
}

void MainWindow::connectParam2() {
    connect(p2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP2);
    connect(p2Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP2);
}

void MainWindow::connectNear() {
    connect(nearSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNearSlider);
    connect(nearBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeNearBox);
}

void MainWindow::connectFar() {
    connect(farSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeFarSlider);
    connect(farBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeFarBox);
}

void MainWindow::connectExtraCredit() {
    connect(ec1, &QCheckBox::clicked, this, &MainWindow::onExtraCredit1);
    connect(ec2, &QCheckBox::clicked, this, &MainWindow::onExtraCredit2);
    connect(ec3, &QCheckBox::clicked, this, &MainWindow::onExtraCredit3);
    connect(ec4, &QCheckBox::clicked, this, &MainWindow::onExtraCredit4);
}

void MainWindow::onPerPixelFilter() {
    settings.perPixelFilter = !settings.perPixelFilter;
    realtime->settingsChanged();
}

void MainWindow::onKernelBasedFilter() {
    settings.kernelBasedFilter = !settings.kernelBasedFilter;
    realtime->settingsChanged();
}

void MainWindow::onUploadFile() {
    // Get abs path of scene file
    QString configFilePath = QFileDialog::getOpenFileName(this, tr("Upload File"), QDir::homePath(), tr("Scene Files (*.xml)"));
    if (configFilePath.isNull()) {
        std::cout << "Failed to load null scenefile." << std::endl;
        return;
    }

    settings.sceneFilePath = configFilePath.toStdString();

    std::cout << "Loaded scenefile: \"" << configFilePath.toStdString() << "\"." << std::endl;

    realtime->sceneChanged();
}

void MainWindow::onValChangeToon1(int newValue) {
    toon1Slider->setValue(newValue);
    toon1Box->setValue(newValue);
    settings.toon1 = toon1Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeToon2(int newValue) {
    toon2Slider->setValue(newValue);
    toon2Box->setValue(newValue);
    settings.toon2 = toon2Slider->value();
    realtime->settingsChanged();
}


void MainWindow::onValChangeP1(int newValue) {
    p1Slider->setValue(newValue);
    p1Box->setValue(newValue);
    settings.shapeParameter1 = p1Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeP2(int newValue) {
    p2Slider->setValue(newValue);
    p2Box->setValue(newValue);
    settings.shapeParameter2 = p2Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearSlider(int newValue) {
    //nearSlider->setValue(newValue);
    nearBox->setValue(newValue/100.f);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeToon3Slider(int newValue) {
    //farSlider->setValue(newValue);
    toon3Box->setValue(newValue/10.f);
    settings.toon3 = toon3Box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeToon3Box(double newValue) {
    toon3Slider->setValue(int(newValue*10.f));
    //nearBox->setValue(newValue);
    settings.toon3 = toon3Box->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarBox(double newValue) {
    farSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}


void MainWindow::onValChangeFarSlider(int newValue) {
    //farSlider->setValue(newValue);
    farBox->setValue(newValue/100.f);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearBox(double newValue) {
    nearSlider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}
// Extra Credit:

void MainWindow::onExtraCredit1() {
    settings.extraCredit1 = !settings.extraCredit1;
    realtime->settingsChanged();
}

void MainWindow::onExtraCredit2() {
    settings.extraCredit2 = !settings.extraCredit2;
    realtime->settingsChanged();
}

void MainWindow::onExtraCredit3() {
    settings.extraCredit3 = !settings.extraCredit3;
    realtime->settingsChanged();
}

void MainWindow::onExtraCredit4() {
    settings.extraCredit4 = !settings.extraCredit4;
    realtime->settingsChanged();
}
