#include "rshelper.h"
#include <QSerialPortInfo>
#include <QTextCodec>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDir>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
void populatePorts(QComboBox *combo);

RSHelper::RSHelper(){
    serial = new QSerialPort(this);

    connect(serial, &QSerialPort::readyRead, this, [this]() {
        data = serial->readAll();

        QTextCodec *codec = QTextCodec::codecForName("IBM866");
        QString message = codec->toUnicode(data);

        setMessage(message);
    });

    // INFO LAYOUT
    QVBoxLayout *messageLayout = new QVBoxLayout();
    output = new QLabel("Received message: none");

    QHBoxLayout *sendLayout = new QHBoxLayout();

    QLineEdit *messageField = new QLineEdit();
    messageField->setPlaceholderText("MESSAGE");
    messageField->setDisabled(true);

    QPushButton *sendBtn = new QPushButton("Send");
    sendBtn->setDisabled(true);
    sendLayout->addWidget(messageField);
    sendLayout->addWidget(sendBtn);

    connect(sendBtn, &QPushButton::clicked, this, [this, messageField](){
        sendMessage(messageField->text());
    });

    connect(messageField, &QLineEdit::textChanged, this, [=](const QString &text){
        if(text.length() > 0)
            sendBtn->setDisabled(false);
        else
            sendBtn->setDisabled(true);
    });

    messageLayout->addWidget(output);
    messageLayout->addLayout(sendLayout);

    QComboBox *portSelector = new QComboBox();

    populatePorts(portSelector);
    if(portSelector->count() > 0)
        setPortName(portSelector->currentText());

    connect(portSelector, &QComboBox::currentTextChanged, this, [this](const QString &text){
        setPortName(text);
    });

    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *infoLayout = new QHBoxLayout();
    QHBoxLayout *configLayout = new QHBoxLayout();

    QVBoxLayout *configInfoLayout = new QVBoxLayout();
    QVBoxLayout *configBtnsLayout = new QVBoxLayout();

    QHBoxLayout *comPortLayout = new QHBoxLayout();

    comPortLayout->addWidget(new QLabel("COM port:"));
    comPortLayout->addWidget(portSelector);

    configInfoLayout->addLayout(comPortLayout);
    configInfoLayout->addWidget(new QLabel(QString("BaudRate: %1").arg(baudRate)));

    QPushButton *connectBtn = new QPushButton("Connect");
    QPushButton *stopBtn = new QPushButton("Stop");
    stopBtn->setDisabled(true);
    configBtnsLayout->addWidget(connectBtn);
    configBtnsLayout->addWidget(stopBtn);

    configLayout->addLayout(configInfoLayout);
    configLayout->addLayout(configBtnsLayout);

    infoLayout->addLayout(messageLayout, 1);
    infoLayout->addStretch(1);
    infoLayout->addLayout(configLayout, 2);

    // BIT SELECT
    QHBoxLayout *bitSelectLayout = new QHBoxLayout();
    QVBoxLayout *selectorLayout = new QVBoxLayout();

    QHBoxLayout *bitConfirmationLayout = new QHBoxLayout();

    QLineEdit *bitSelect = new QLineEdit();
    bitSelect->setFixedWidth(45);
    bitSelect->setMaxLength(3);
    bitSelect->setPlaceholderText("0");
    bitSelect->setDisabled(true);

    QPushButton *bitConfirmation = new QPushButton("Check");
    bitConfirmation->setDisabled(true);

    connect(bitSelect, &QLineEdit::textChanged, this, [=](const QString &text){
        if(text.length() > 0)
            bitConfirmation->setDisabled(false);
        else
            bitConfirmation->setDisabled(true);
    });


    bitConfirmationLayout->addWidget(bitSelect);
    bitConfirmationLayout->addWidget(bitConfirmation);

    connect(bitConfirmation, &QPushButton::clicked, this, [this, bitSelect]{
        emit bitSelected(data[bitSelect->text().toInt()]);
    });

    selectorLayout->addWidget(new QLabel("Bit:"));
    selectorLayout->addLayout(bitConfirmationLayout);

    errorMessage = new QLabel("No Errors detected.");
    errorMessage->setStyleSheet("color: darkGreen");
    errorMessage->setAlignment(Qt::AlignRight);

    bitSelectLayout->addLayout(selectorLayout, 1);
    bitSelectLayout->addStretch(1);
    bitSelectLayout->addWidget(errorMessage, 3);

    layout->addLayout(bitSelectLayout);
    layout->addLayout(infoLayout);

    setLayout(layout);

    connect(connectBtn, &QPushButton::clicked, this, [this, connectBtn, stopBtn, portSelector, sendBtn, bitSelect, messageField, bitConfirmation]{
        if(openPort() >= 0){
            connectBtn->setDisabled(true);
            portSelector->setDisabled(true);

            if(bitSelect->text().length() > 0)
                bitConfirmation->setDisabled(false);

            if(messageField->text().length() > 0)
                sendBtn->setDisabled(false);

            messageField->setDisabled(false);
            bitSelect->setDisabled(false);
            stopBtn->setDisabled(false);
        }
    });

    connect(stopBtn, &QPushButton::clicked, this, [this, connectBtn, stopBtn, portSelector, sendBtn, bitSelect, messageField, bitConfirmation]{
        if(closePort() >= 0){
            connectBtn->setDisabled(false);
            portSelector->setDisabled(false);

            bitConfirmation->setDisabled(true);
            messageField->setDisabled(true);
            bitSelect->setDisabled(true);
            sendBtn->setDisabled(true);
            stopBtn->setDisabled(true);
        }
    });
}

void RSHelper::sendMessage(QString sentMessage){

    QTextCodec *codec = QTextCodec::codecForName("IBM866");
    QByteArray sentData = codec->fromUnicode(sentMessage);

    qint64 bytesWritten = serial->write(sentData);

    if(bytesWritten == -1){
        updateError("Error: Write failed.");
        return;
    }
    updateError();
}

int RSHelper::openPort(){
    serial->setPortName(m_portName);
    serial->setBaudRate(baudRate);
    serial->setParity(QSerialPort::EvenParity);
    serial->setStopBits(QSerialPort::TwoStop);
    serial->setDataBits(QSerialPort::Data8);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!serial->open(QIODevice::ReadWrite)) {
        updateError("Error: could not open the port.");
        return -1;
    }
    serial->clear();
    updateError();

    output->setText("Received message: none");

    return 0;
}

int RSHelper::closePort(){
    serial->flush();
    serial->close();
    return 0;
}

void RSHelper::updateError(QString message){
    if(message == nullptr){
        errorMessage->setText("No Errors detected.");
        errorMessage->setStyleSheet("color: darkGreen");
    } else{
        errorMessage->setText(message);
        errorMessage->setStyleSheet("color: darkRed");
    }
}

void populatePorts(QComboBox *combo)
{
    combo->clear();

    QDir dir("/tmp");
    QStringList filters;
    filters << "ttyV*";
    QFileInfoList list = dir.entryInfoList(filters, QDir::System | QDir::Readable);

    for (const QFileInfo &fi : list) {
        combo->addItem(fi.filePath());
    }

    if (combo->count() == 0) {
        combo->addItem("No ports found");
    }
}



