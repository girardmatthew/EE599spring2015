#include "lauaudiofilterobject.h"

int LAUAudioFilterObject::instanceCounter = 0;

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
LAUAudioFilterObject::LAUAudioFilterObject(QString filename, bool log, QObject *parent) : QObject(parent), sampleCounter(0), threshold(1.0f)
{
    instance = instanceCounter;
    instanceCounter++;

    inFile = NULL;
    otFile = NULL;
    if (log){
        QString location = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        //inFile = new QFile(QString("%1/inData%2.dat").arg(location).arg(instance));
        //inFile->open(QIODevice::WriteOnly);
        otFile = new QFile(QString("%1/otData%2.dat").arg(location).arg(instance));
        otFile->open(QIODevice::WriteOnly);
    }

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)){
        while (file.atEnd() == false){
            QByteArray byteArray = file.readLine();
            if (byteArray.contains("SOS Matrix:")){
                while(1){
                    // ITERATIVELY READ LINES OF DATA IN UNTIL THERE ARE NO MORE FILTERS
                    QStringList filterStringList = QString(file.readLine()).simplified().split(" ", QString::SkipEmptyParts);
                    if (filterStringList.count() == 6){
                        double coefficients[6];
                        bool okay=true;
                        for (int n=0; n<6; n++){
                            bool ok;
                            coefficients[n] = filterStringList.at(n).toDouble(&ok);
                            okay = okay & ok;
                        }
                        if (okay){
                            filterList << LAUBiQuadFilter(coefficients);
                        }
                    } else {
                        break;
                    }
                }
            } else if (byteArray.contains("Scale Values:")){
                for (int n=0; n<filterList.length(); n++){
                    QStringList filterStringList = QString(file.readLine()).simplified().split(" ", QString::SkipEmptyParts);
                    if (filterStringList.count() == 1){
                        bool okay=true;
                        double scale = filterStringList.takeFirst().toDouble(&okay);
                        if (okay){
                            filterList[n].setScaleFactor(scale);
                        }
                    } else {
                        break;
                    }
                }
            }
        }
    }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void LAUAudioFilterObject::onProcessBuffer(float *buffer, int samples)
{
    if (inFile) inFile->write((char*)buffer, samples*sizeof(float));
    bool flag = false;
    for (int m=0; m<samples; m++){
        double x = (double)buffer[m];
        for (int n=0; n<filterList.count(); n++){
            x = filterList[n].filter(x);
        }
        if (qAbs(x) > threshold) { flag = true; }
        buffer[m] = (float)x;
    }
    if (otFile) otFile->write((char*)buffer, samples*sizeof(float));
    sampleCounter += samples;
    emit emitBuffer(buffer, samples);
    if (flag) emit emitTrigger();
}
