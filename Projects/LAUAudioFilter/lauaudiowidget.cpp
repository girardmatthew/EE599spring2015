#include "lauaudiowidget.h"

#ifdef RECORDDEBUGDATA
#define RECORDFLAG  true
#else
#define RECORDFLAG  false
#endif

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
LAUAudioWidget::LAUAudioWidget(QWidget *parent) : QWidget(parent)
{
    this->setMinimumHeight(480);
    this->setMinimumWidth(640);

    // CREATE A GLWIDGET TO DISPLAY LIVE AUDIO
    QGLFormat glFormat;
    glFormat.setVersion(4, 1);
    glFormat.setSampleBuffers(true);
    glFormat.setProfile(QGLFormat::CoreProfile);

    audiGLWidget = new LAUAudioGLWidget(glFormat);
    audiGLWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    maleGLWidget = new LAUAudioGLWidget(glFormat);
    maleGLWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    femaGLWidget = new LAUAudioGLWidget(glFormat);
    femaGLWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ADD THE AUDIO WIDGET TO THIS WIDGET'S LAYOUT
    this->setLayout(new QVBoxLayout());
    this->layout()->setContentsMargins(0,0,0,0);

    QGroupBox *groupBox = new QGroupBox(QString("Audio"));
    groupBox->setLayout(new QHBoxLayout());
    groupBox->layout()->setContentsMargins(6,6,6,6);
    groupBox->layout()->addWidget(audiGLWidget);
    mastSlider = new QSlider();
    mastSlider->setMaximum(100);
    mastSlider->setMinimum(0);
    groupBox->layout()->addWidget(mastSlider);
    this->layout()->addWidget(groupBox);

    groupBox = new QGroupBox(QString("Male"));
    groupBox->setLayout(new QHBoxLayout());
    groupBox->layout()->setContentsMargins(6,6,6,6);
    groupBox->layout()->addWidget(maleGLWidget);
    maleSlider = new QSlider();
    maleSlider->setMaximum(100);
    maleSlider->setMinimum(0);
    groupBox->layout()->addWidget(maleSlider);
    this->layout()->addWidget(groupBox);

    groupBox = new QGroupBox(QString("Female"));
    groupBox->setLayout(new QHBoxLayout());
    groupBox->layout()->setContentsMargins(6,6,6,6);
    groupBox->layout()->addWidget(femaGLWidget);
    femaSlider = new QSlider();
    femaSlider->setMaximum(100);
    femaSlider->setMinimum(0);
    groupBox->layout()->addWidget(femaSlider);
    this->layout()->addWidget(groupBox);

    groupBox = new QGroupBox(QString("Levels"));
    groupBox->setLayout(new QHBoxLayout());
    groupBox->layout()->setContentsMargins(6,6,6,6);

    // ADD AND CONNECT THE TEST ZAP BUTTON
    QPushButton *zapButton = new QPushButton("Trigger", this);
    groupBox->layout()->addWidget(zapButton);

    // ADD AND CONNECT THE ZAPPER RESET BUTTON
    QPushButton *resetButton = new QPushButton("Reset", this);
    groupBox->layout()->addWidget(resetButton);
    ((QHBoxLayout*)groupBox->layout())->addStretch();

    mastSpinBox = new QSpinBox();
    mastSpinBox->setMinimum(0);
    mastSpinBox->setMaximum(100);
    mastSpinBox->setValue(100);
    QLabel *label = new QLabel(QString("Master:"));
    groupBox->layout()->addWidget(label);
    groupBox->layout()->addWidget(mastSpinBox);
    maleSpinBox = new QSpinBox();
    maleSpinBox->setMinimum(0);
    maleSpinBox->setMaximum(100);
    maleSpinBox->setValue(100);
    label = new QLabel(QString("Male:"));
    groupBox->layout()->addWidget(label);
    groupBox->layout()->addWidget(maleSpinBox);
    femaSpinBox = new QSpinBox();
    femaSpinBox->setMinimum(0);
    femaSpinBox->setMaximum(100);
    femaSpinBox->setValue(100);
    label = new QLabel(QString("Female:"));
    groupBox->layout()->addWidget(label);
    groupBox->layout()->addWidget(femaSpinBox);
    this->layout()->addWidget(groupBox);

    // CREATE AUDIO THREAD AND AUDIO INPUT DEVICE OBJECTS FOR RECORDING AUDIO
    audioThread = new QThread();

    audioObject = new LAUAudioObject();
    audioObject->moveToThread(audioThread);
    connect(audioThread, SIGNAL(started()), audioObject, SLOT(onStart()));
    connect(audioThread, SIGNAL(finished()), audioObject, SLOT(deleteLater()));

    for (int n=0; n<3; n++){
        audioFilterThreads[n] = new QThread();
        if (n==0){
            audioFilterObjects[n] = new LAUAudioDownSampler(RECORDFLAG);
        } else if (n==1) {
            audioFilterObjects[n] = new LAUAudioFilterObject(QString(":/filters/maleFilter.fcf"), RECORDFLAG);
        } else if (n==2) {
            audioFilterObjects[n] = new LAUAudioFilterObject(QString(":/filters/femaleFilter.fcf"), RECORDFLAG);
        }
        audioFilterObjects[n]->moveToThread(audioFilterThreads[n]);
    }

    // CONNECT DOWNSAMPLED SIGNAL TO BANDPASS FILTERED SIGNAL
    connect(audioObject,           SIGNAL(emitBuffer(float*,int)),   audiGLWidget,          SLOT(onUpdateBuffer(float*,int)),  Qt::QueuedConnection);
    connect(audiGLWidget,          SIGNAL(emitBuffer(float*,int)),   audioFilterObjects[0], SLOT(onProcessBuffer(float*,int)), Qt::QueuedConnection);
    connect(audioFilterObjects[0], SIGNAL(emitBufferLo(float*,int)), audioFilterObjects[1], SLOT(onProcessBuffer(float*,int)), Qt::QueuedConnection);
    connect(audioFilterObjects[0], SIGNAL(emitBufferHi(float*,int)), audioFilterObjects[2], SLOT(onProcessBuffer(float*,int)), Qt::QueuedConnection);
    connect(audioFilterObjects[1], SIGNAL(emitBuffer(float*,int)),   maleGLWidget,          SLOT(onUpdateBuffer(float*,int)),  Qt::QueuedConnection);
    connect(audioFilterObjects[2], SIGNAL(emitBuffer(float*,int)),   femaGLWidget,          SLOT(onUpdateBuffer(float*,int)),  Qt::QueuedConnection);

    // CONNECT SLIDERS TO THRESHOLDS
    connect(mastSpinBox, SIGNAL(valueChanged(int)),   mastSlider, SLOT(setValue(int)));
    connect( mastSlider, SIGNAL(valueChanged(int)),  mastSpinBox, SLOT(setValue(int)));
    connect( mastSlider, SIGNAL(valueChanged(int)), audiGLWidget, SLOT(onSetThreshold(int)));

    connect(maleSpinBox, SIGNAL(valueChanged(int)),   maleSlider, SLOT(setValue(int)));
    connect( maleSlider, SIGNAL(valueChanged(int)),  maleSpinBox, SLOT(setValue(int)));
    connect( maleSlider, SIGNAL(valueChanged(int)), maleGLWidget, SLOT(onSetThreshold(int)));
    connect( maleSlider, SIGNAL(valueChanged(int)), audioFilterObjects[1], SLOT(onSetThreshold(int)));

    connect(femaSpinBox, SIGNAL(valueChanged(int)),   femaSlider, SLOT(setValue(int)));
    connect( femaSlider, SIGNAL(valueChanged(int)),  femaSpinBox, SLOT(setValue(int)));
    connect( femaSlider, SIGNAL(valueChanged(int)), femaGLWidget, SLOT(onSetThreshold(int)));
    connect( femaSlider, SIGNAL(valueChanged(int)), audioFilterObjects[2], SLOT(onSetThreshold(int)));

    // TELL THE AUDIO THREAD TO START RECORDING AUDIO
    for (int n=0; n<3; n++){
        audioFilterThreads[n]->start();
    }
    audioThread->start();

    // LOAD THRESHOLDS FROM PREVIOUS RUN
    QSettings settings;
    mastSpinBox->setValue(settings.value(QString("mastSpinBox"), 100).toInt());
    maleSpinBox->setValue(settings.value(QString("maleSpinBox"), 100).toInt());
    femaSpinBox->setValue(settings.value(QString("femaSpinBox"), 100).toInt());
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
LAUAudioWidget::~LAUAudioWidget()
{
    for (int n=0; n<3; n++){
        audioFilterThreads[n]->quit();
        while (audioFilterThreads[n]->isRunning()){
            qApp->processEvents();
        }
        delete audioFilterThreads[n];
        delete audioFilterObjects[n];
    }
    audioThread->quit();
    while (audioThread->isRunning()){
        qApp->processEvents();
    }
    delete audioThread;

    // SAVE AUDIO THRESHOLD LEVELS
    QSettings settings;
    settings.setValue(QString("mastSpinBox"), mastSpinBox->value());
    settings.setValue(QString("maleSpinBox"), maleSpinBox->value());
    settings.setValue(QString("femaSpinBox"), femaSpinBox->value());

    qDebug() << QString("LAUAudioWidget::~LAUAudioWidget()");
}
