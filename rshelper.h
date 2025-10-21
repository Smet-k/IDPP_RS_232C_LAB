#ifndef RSHELPER_H
#define RSHELPER_H
#include <QString>
#include <QSerialPort>
#include <QString>
#include <QWidget>
#include <QLabel>

class RSHelper : public QWidget {
    Q_OBJECT
public:
    RSHelper();
    int openPort();
    void sendMessage(QString message);
    int closePort();

    void openPort(QString portName);

    QString message() {return m_message; }
    void setMessage(const QString message) {
        m_message = message;
        output->setText(QString("Received message: %1").arg(message));
    }

    QString portName() {return m_portName; }
    void setPortName(const QString portName) {m_portName = portName; }
private:
    void updateError(QString message = nullptr);

    const int baudRate = 150;
    QByteArray data;
    QString m_portName;

    QLabel *errorMessage;
    QSerialPort *serial;
    QString m_message;
    QLabel *output;
signals:
    void bitSelected(unsigned char bit);
};

#endif // RSHELPER_H
