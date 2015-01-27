#include "lauaudioobject.h"

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
LAUAudioObject::LAUAudioObject(QObject *parent) : QObject(parent), sampleCounter(0), buffer(NULL), conversionBuffer(NULL), audioInput(NULL), audioDevice(NULL)
{
    // SET THE AUDIO FORMAT FOR RECORDING
    format.setSampleRate(44100);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setChannelCount(1);
    format.setCodec(QString("audio/pcm"));
    format.setSampleSize(32);
    format.setSampleType(QAudioFormat::Float);

    // ASK THE USER TO SPECIFY THE INPUT AUDIO DEVICE IF MORE THAN ONE EXISTS
    QList<QAudioDeviceInfo>	deviceList = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    if (deviceList.count() > 1){
        QStringList deviceStringList;
        for (int n=0; n<deviceList.count(); n++){
            deviceStringList << deviceList.at(n).deviceName();
        }
        QString deviceString = QInputDialog::getItem(0, QString("LAU Audio Tool"), QString("Select input audio device."), deviceStringList, 0, false);
        audioDeviceInfo = deviceList.at(deviceStringList.indexOf(deviceString));
    } else if (deviceList.count() == 1){
        audioDeviceInfo = deviceList.at(0);
    }

    if (isValid() == false){
        format.setSampleSize(16);
        format.setSampleType(QAudioFormat::SignedInt);
        if (isValid() == false){
            format.setSampleSize(8);
            format.setSampleType(QAudioFormat::SignedInt);
        }
    }
    // CODE FOR TESTING THE AVAILABLE AUDIO HARDWARE
    //QList<QAudioFormat::Endian> byteOrders = audioDeviceInfo.supportedByteOrders();
    //QList<int> channelCounts = audioDeviceInfo.supportedChannelCounts();
    //QStringList codecs = audioDeviceInfo.supportedCodecs();
    //QList<int> sampleRates = audioDeviceInfo.supportedSampleRates();
    //QList<int> sampleSizes = audioDeviceInfo.supportedSampleSizes();
    //QList<QAudioFormat::SampleType> sampleTypes = audioDeviceInfo.supportedSampleTypes();
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
LAUAudioObject::~LAUAudioObject()
{
    if (buffer) free(buffer);
    if (conversionBuffer) free(conversionBuffer);
    if (audioInput) {
        audioInput->stop();
        delete audioInput;
    }
    qDebug() << QString("LAUAudioObject::~LAUAudioObject()");
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
bool LAUAudioObject::isValid()
{
    return(audioDeviceInfo.isFormatSupported(format));
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void LAUAudioObject::onStart()
{
    // MAKE SURE WE HAVE A VALID AUDIO INPU DEVICE TO WORK WITH
    if (isValid()){
        // CREATE A LOCAL BUFFER TO HOLD INCOMING SAMPLES
        buffer = (float*)malloc((AUDIOBUFFERLENGTH+32000)*sizeof(float));
        memset(buffer, 0, (AUDIOBUFFERLENGTH+32000)*sizeof(float));

        sampleCounter = 0;                            // INITIALIZE A VARIABLE TO KEEP TRACK OF WHERE THE NEWEST SAMPLES SHOULD BE SAVED IN OUR RECORDING BUFFER
        audioInput = new QAudioInput(format);         // CREATE OUR AUDIO INPUT OBJECT
        audioInput->setBufferSize(44100);             // SET THE INPUT OBJECT'S BUFFER SIZE
        audioInput->setNotifyInterval(50);            // SET THE INPUT OBJECT'S NOTIFY RATE

        // CONNECT THE AUDIO INPUTS NOTIFY SIGNAL TO THIS CLASS'S NOTIFY SLOT
        connect(audioInput, SIGNAL(notify()), this, SLOT(onNotify()));

        // TELL THE AUDIO INPUT DEVICE TO START RECORDING AUDIO SAMPLES
        audioDevice = audioInput->start();

        // CHECK TO SEE IF WE NEED TO CREATE A CONVERSION BUFFER
        if (format.sampleType() == QAudioFormat::SignedInt){
            conversionBuffer = (short*)malloc((AUDIOBUFFERLENGTH+32000)*sizeof(short));
            memset(conversionBuffer, 0, (AUDIOBUFFERLENGTH+32000)*sizeof(short));
        }
    }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
void LAUAudioObject::onNotify()
{
    while (audioInput->bytesReady() > 0){
        int samples = 0;
        if (format.sampleType() == QAudioFormat::Float){
            // GRAB ALL AVAILABLE SAMPLES AT ONE TIME
            samples = audioDevice->read((char*)&buffer[sampleCounter], 32000*sizeof(float))/4;
        } else if (format.sampleType() == QAudioFormat::SignedInt){
            // GRAB ALL AVAILABLE SAMPLES AT ONE TIME
            samples = audioDevice->read((char*)&conversionBuffer[sampleCounter], 32000*sizeof(float))/2;
            for (int n=0; n<samples; n++){
                buffer[sampleCounter+n] = (float)conversionBuffer[sampleCounter+n]/32767.0f;
            }
        }
        // SEND THE JUST RECEIVED SAMPLES TO THE USER FOR PROCESSING
        emit emitBuffer(&buffer[sampleCounter], samples);

        // NOW DECIDE IF WE NEED TO WRITE INCOMING SAMPLES BACK TO THE BEGINNING OF THE RECORDING BUFFER
        if (sampleCounter+samples >= AUDIOBUFFERLENGTH){
            sampleCounter = 0;
        } else {
            sampleCounter += samples;
        }
    }
}
