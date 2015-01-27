#ifndef LAUGPIOOBJECT_H
#define LAUGPIOOBJECT_H

#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QObject>
#include <QFileInfo>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#define BAUD QSerialPort::Baud19200

//#define SHOW_PORTS

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
class LAUGPIOObject : public QObject
{
    Q_OBJECT

public:
    explicit LAUGPIOObject(QObject *parent = 0);
    ~LAUGPIOObject();

    bool isValid() const;   // CALL THIS FUNCTION TO SEE IF WE CONNECTED TO MODULE
    bool isZapping() const; // CALL THIS TO TEST IF ZAPPER IS ON

public slots:
    void onZap(int timeout = 2);  // CALL THIS TO ZAP FOR timeout MILLISECONDS
    void onReset();               // CALL THIS TO PULL DTR LOW AND RESET THE BOARD

private:
    QSerialPort* serial;
    bool zapping, isPort;
    int eventCounter;

signals:

private slots:
    void onZappingComplete();
};

#endif // LAUGPIOOBJECT_H
