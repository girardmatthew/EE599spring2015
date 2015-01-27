#ifndef LAUAUDIOFILTEROBJECT_H
#define LAUAUDIOFILTEROBJECT_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QStandardPaths>
#include <QStringList>
#include "lauaudioobject.h"

class LAUBiQuadFilter;
class LAUAudioDownSampler;
class LAUAudioFilterObject;

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
class LAUBiQuadFilter
{
public:
    LAUBiQuadFilter()
    {
        sc = 1;
        a[0] = 1; a[1] = 0; a[2] = 0;
        b[0] = 0; b[1] = 0; b[2] = 0;
        x[0] = 0; x[1] = 0; x[2] = 0;
        y[0] = 1; y[1] = 0; y[2] = 0;
    }

    LAUBiQuadFilter(double coef[], double s = 1.0)
    {
        sc = s;
        x[0] = 0;       x[1] = 0;       x[2] = 0;
        y[0] = 0;       y[1] = 0;       y[2] = 0;
        a[0] = coef[3]; a[1] = coef[4]; a[2] = coef[5];
        b[0] = coef[0]; b[1] = coef[1]; b[2] = coef[2];
    }

    LAUBiQuadFilter(double b0, double b1, double b2, double a0, double a1, double a2, double s = 1.0)
    {
        sc = s;
        x[0] = 0;  x[1] = 0;  x[2] = 0;
        y[0] = 0;  y[1] = 0;  y[2] = 0;
        a[0] = a0; a[1] = a1; a[2] = a2;
        b[0] = b0; b[1] = b1; b[2] = b2;
    }

    void setScaleFactor(double s) { sc = s; }

    double filter(double in)
    {
        x[2] = x[1]; x[1] = x[0]; x[0] = sc*in;
        y[2] = y[1]; y[1] = y[0];
        y[0] = (x[0] + b[1]*x[1] + b[2]*x[2] - a[1]*y[1] - a[2]*y[2]);

        return(y[0]);
    }

private:
    double a[3], b[3];
    double x[3], y[3];
    double sc;
};

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
class LAUAudioFilterObject : public QObject
{
    Q_OBJECT

public:
    LAUAudioFilterObject(QString filename, bool log = true, QObject *parent = 0);
    ~LAUAudioFilterObject()
    {
        if (inFile) { inFile->close(); delete inFile; }
        if (otFile) { otFile->close(); delete otFile; }
        qDebug() << QString("LAUAudioFilterObject::~LAUAudioFilterObject()");
    }

public slots:
    void onSetThreshold(int val) { onSetThreshold((float)val/100.0f); }
    void onSetThreshold(float val) { threshold = val; }
    void onProcessBuffer(float *buffer, int samples);

protected:
    QFile *inFile;
    QFile *otFile;
    static int instanceCounter;
    QVector<LAUBiQuadFilter> filterList;
    int sampleCounter;
    int instance;
    float threshold;

signals:
    void emitBuffer(float *buffer, int samples);
    void emitTrigger();
};

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
class LAUAudioDownSampler : public LAUAudioFilterObject
{
    Q_OBJECT

public:
    LAUAudioDownSampler(bool log = true, QObject *parent = 0) : LAUAudioFilterObject(QString(":/filters/lowPassFilter.fcf"), log, parent) { ; }

public slots:
    void onProcessBuffer(float *buffer, int samples){
        if (inFile) inFile->write((char*)buffer, samples*sizeof(float));
        for (int m=0; m<samples; m++){
            double x = (double)buffer[m];
            for (int n=0; n<filterList.count(); n++){
                x = filterList[n].filter(x);
            }
            buffer[m/5] = (float)x;
            sampleCounter++;
        }
        if (otFile) otFile->write((char*)buffer, samples/5*sizeof(float));
        for (int m=0; m<samples/5; m++){
            buffer[m + samples/2] = buffer[m];
        }
        emit emitBufferLo(&buffer[0], samples/5);
        emit emitBufferHi(&buffer[samples/2], samples/5);
    }

signals:
    void emitBufferLo(float *buffer, int samples);
    void emitBufferHi(float *buffer, int samples);
};

#endif // LAUAUDIOFILTEROBJECT_H
