#ifndef LAUAUDIOWIDGET_H
#define LAUAUDIOWIDGET_H

#include <QFile>
#include <QList>
#include <QDebug>
#include <QLabel>
#include <QSlider>
#include <QWidget>
#include <QString>
#include <QThread>
#include <QDialog>
#include <QSpinBox>
#include <QSettings>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QDialogButtonBox>

#include "lauaudioobject.h"
#include "lauaudioglwidget.h"
#include "lauaudiofilterobject.h"

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
class LAUAudioWidget : public QWidget
{
    Q_OBJECT

public:
    LAUAudioWidget(QWidget *parent = 0);
    ~LAUAudioWidget();

private:
    QThread *audioThread;
    QThread* audioFilterThreads[3];
    QSlider *femaSlider, *maleSlider, *mastSlider;
    QSpinBox *femaSpinBox, *maleSpinBox, *mastSpinBox;

    LAUAudioObject *audioObject;
    LAUAudioFilterObject* audioFilterObjects[3];
    LAUAudioGLWidget *audiGLWidget;
    LAUAudioGLWidget *maleGLWidget;
    LAUAudioGLWidget *femaGLWidget;
};

class LAUAudioDialog : public QDialog
{
    Q_OBJECT

public:
    LAUAudioDialog(QDialog *parent = 0) : QDialog(parent)
    {
        this->setLayout(new QVBoxLayout());
        this->layout()->setContentsMargins(6,6,6,6);

        audioWidget = new LAUAudioWidget();
        this->layout()->addWidget(audioWidget);
        ((QVBoxLayout*)(this->layout()))->addSpacing(20);

        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
        this->layout()->addWidget(buttonBox);
        connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(accept()));
        connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
    }

protected:
    void accept() { QDialog::accept(); }
    void reject() { QDialog::reject(); }

public slots:

private:
    LAUAudioWidget *audioWidget;

private slots:

signals:

};



#endif // LAUAUDIOWIDGET_H
