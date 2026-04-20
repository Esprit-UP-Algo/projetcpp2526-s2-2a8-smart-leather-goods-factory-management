#include "arduino.h"
#include <QDebug>

Arduino::Arduino()
{
    serial = new QSerialPort();
    arduino_available = false;
    arduino_port_name = "";
}

Arduino::~Arduino()
{
    if (serial->isOpen())
        serial->close();
    delete serial;
}

QString Arduino::findArduinoPort()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (info.hasVendorIdentifier() && info.hasProductIdentifier()) {
            if (info.vendorIdentifier() == 9025) {
                return info.portName();
            }
        }
    }
    
    QStringList commonPorts = {"COM3", "COM4", "COM5", "COM6", "COM7", "COM8"};
    foreach (const QString &portName, commonPorts) {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            if (info.portName() == portName) {
                return portName;
            }
        }
    }
    
    return "";
}

int Arduino::connect_arduino()
{
    arduino_port_name = findArduinoPort();
    
    if (arduino_port_name.isEmpty()) {
        qDebug() << "Aucun Arduino detecte";
        return -1;
    }
    
    serial->setPortName(arduino_port_name);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    
    if (serial->open(QIODevice::ReadWrite)) {
        arduino_available = true;
        qDebug() << "Arduino connecte sur" << arduino_port_name;
        return 0;
    } else {
        qDebug() << "Erreur connexion:" << serial->errorString();
        return -1;
    }
}

void Arduino::close_arduino()
{
    if (serial->isOpen()) {
        serial->close();
        arduino_available = false;
        qDebug() << "Arduino deconnecte";
    }
}

bool Arduino::isConnected() const
{
    return arduino_available && serial->isOpen();
}

int Arduino::write_to_arduino(const QByteArray &data)
{
    if (serial->isOpen()) {
        qint64 written = serial->write(data);
        serial->flush();
        return written;
    }
    return -1;
}

QByteArray Arduino::read_from_arduino()
{
    if (serial->isOpen() && serial->waitForReadyRead(100)) {
        return serial->readAll();
    }
    return QByteArray();
}

QString Arduino::getPortName() const
{
    return arduino_port_name;
}
