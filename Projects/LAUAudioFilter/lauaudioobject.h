#ifndef LAUAUDIOOBJECT_H
#define LAUAUDIOOBJECT_H

#include <QFile>
#include <QDebug>
#include <QObject>
#include <QAudioInput>
#include <QStringList>
#include <QInputDialog>
#include <QAudioFormat>
#include <QAudioDeviceInfo>

#ifndef AUDIOBUFFERLENGTH
#define AUDIOBUFFERLENGTH 100000
#endif

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
class LAUAudioObject : public QObject
{
    Q_OBJECT

public:
    LAUAudioObject(QObject *parent = 0);
    ~LAUAudioObject();

    bool isValid();

public slots:
    void onStart();
    void onNotify();

private:
    float *buffer;
    short *conversionBuffer;
    int sampleCounter;
    QAudioFormat format;
    QAudioDeviceInfo audioDeviceInfo;
    QAudioInput *audioInput;
    QIODevice *audioDevice;

signals:
    void emitBuffer(float *buffer, int samples);
};

#endif // LAUAUDIOOBJECT_H
