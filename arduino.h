#ifndef ARDUINO_H
#define ARDUINO_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QByteArray>

class Arduino
{
public:
    Arduino();
    ~Arduino();
    
    int connect_arduino();
    void close_arduino();
    bool isConnected() const;
    
    int write_to_arduino(const QByteArray &data);
    QByteArray read_from_arduino();
    
    QString getPortName() const;

private:
    QSerialPort *serial;
    bool arduino_available;
    QString arduino_port_name;
    
    QString findArduinoPort();
};

#endif
