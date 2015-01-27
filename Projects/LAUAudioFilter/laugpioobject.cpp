#include "laugpioobject.h"

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
LAUGPIOObject::LAUGPIOObject(QObject *parent) : QObject(parent), zapping(false), isPort(false), eventCounter(0), serial(NULL)
{
    QByteArray query(1, '?');
    QByteArray resp("ZAPPER");

    isPort = false;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
#define SHOW_PORTS
#ifdef SHOW_PORTS
        qDebug() << "Name        : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
        qDebug() << "Location    : " << info.systemLocation();
#endif
        if (info.portName().contains(QString("Bluetooth"))) continue;

        serial = new QSerialPort();
        serial->setPortName(info.systemLocation());
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setParity(QSerialPort::NoParity);
        serial->setDataBits(QSerialPort::Data8);
        serial->setBaudRate(BAUD);

        if (serial->open(QIODevice::ReadWrite)) {
            qDebug() << "opened " << info.portName();

            int counter = 0;
            do {
                serial->write(query);
                counter++;
            } while (!serial->waitForReadyRead(1000) && counter < 5);

            QByteArray q = serial->readAll();
            qDebug() << "response: " << q.constData() << ", " << q.size();
            if (q == resp) {
                qDebug() << "FOUND ZAPPER";
                isPort = true;
                break;
            } else {
                serial->close();
                delete serial;
                serial = NULL;
            }
        } else {
            serial->close();
            delete serial;
            serial = NULL;
        }
    }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
LAUGPIOObject::~LAUGPIOObject()
{
    if (serial) {
        qDebug() << "~LAUGPIOObject: closing serial";
        serial->close();
        delete serial;
    }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void LAUGPIOObject::onZap(int timeout)
{
    if (!serial) {
        eventCounter++;
        qDebug() << "NO PORT OPEN, NOT ZAPPING" << eventCounter;
        return;
    }

    // MAKE SURE WE AREN'T ALREADY ZAPPING BEFORE WE TRIGGER ANOTHER
    if (zapping == false){
        eventCounter++;
        zapping = true;

        // MAKE SURE TIMEOUT FALLS WITHIN BOUNDS
        timeout = qMax(qMin(timeout, 9), 1);

        char b = static_cast<char>(timeout) + 48;

        serial->write(&b, 1);
        serial->flush();
        QTimer::singleShot(timeout*100, this, SLOT(onZappingComplete()));
    }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void LAUGPIOObject::onReset()
{
    if (isValid()){
        serial->setDataTerminalReady(true);
        serial->setDataTerminalReady(false);
    }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
bool LAUGPIOObject::isZapping() const
{
    return zapping;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
bool LAUGPIOObject::isValid() const
{
    if (serial && serial->isOpen() && serial->isWritable()){
        return(true);
    } else {
        return(false);
    }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void LAUGPIOObject::onZappingComplete()
{
    zapping = false;
}
