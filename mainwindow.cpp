#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError)));

    socket->connectToHost("127.0.0.1", 6001);
    socket->QTcpSocket::waitForConnected(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotError(QAbstractSocket::SocketError err)
{
    QString str =
            (err == QAbstractSocket::HostNotFoundError) ?
                "The host was not found." :
                (err == QAbstractSocket::RemoteHostClosedError) ?
                    "The remote host is closed." :
                    (err == QAbstractSocket::ConnectionRefusedError) ?
                        "The connection was refused." :
                        QString(socket->errorString());

    QMessageBox::information(this,"ERROR: ", str);
}

void MainWindow::disconnected()
{
    socket ->deleteLater();
}

void MainWindow::readyRead()
{
    qint64 bytesAvailable1 = socket->bytesAvailable();
    qDebug() << "Bytes available1: " << bytesAvailable1;
    dataIn = socket->readAll();
    parsing();
}

void MainWindow::parsing()
{
    QString str;
    int8_t x = 0;
    int i = 0;
    int16_t L = 0;

    switch (i)
    {
    case (0):
    {
        if (dataIn[i] == 'S') i++;
        else
        {
            qDebug() << "Error parsing!";
            QMessageBox::information(this,"Ошибка", "Неправильный старт-бит");
            return;
        }
    }
    case (1):
    {
        L = (dataIn[i+1]<<8) + dataIn[i];
        i +=2;
    }
    case (3):
    {
        if (dataIn[1] + dataIn[2] + dataIn[i] == 0) i++;
        else
        {
            qDebug()<< "Error CRC";
            QMessageBox::information(this,"Ошибка", "Ошибка контрльной суммы длинны данных");
        }
    }
    case(4):
    {
        for(int j = i; j < (i + (int)L); j++)
        {
            str.push_back((char)dataIn[j]);
            x +=dataIn[j];
        }

        i +=((int)L);
    }
    }
    if (x + dataIn[i] != 0)
    {
        qDebug()<< "Error CRC";
        QMessageBox::information(this,"Ошибка", "Ошибка контрльной суммы данных");
    }
    else
    {
        if (str.indexOf('v')==0)
        {
            str.insert(1, QString("ersion: "));
        }
        ui->textEdit->append(str);
    }
}

QByteArray MainWindow::Structure(char p)
{
    int8_t x = 0;
    QByteArray data;
    if (p == 'v')
    {
        data.push_back('v');
        data.push_back('0');
    }

    int16_t datasize = data.length();

    QByteArray pack;
    pack.push_back(0x53); // Старт бит
    pack.push_back(datasize & 0xff); // Младший байт длинны
    pack.push_back(datasize >> 8); // Старший байт длинны
    pack.push_back(0xff - pack[1] - pack[2] + 0x01); // Контр сумма длины

    for (int i = 0; i < data.length(); i++) // Массив данных
    {
        pack.push_back(quint8(data[i]));
        x += data[i];
    }

    pack.push_back(0xff - x + 0x01);  // Контр сумма данных
    return pack;
}

void MainWindow::on_pushButton_clicked()
{
    close();
}


void MainWindow::on_pushButton_2_clicked()
{
    socket->QTcpSocket::waitForBytesWritten(1000);
    QByteArray dataOut = Structure('v');
    socket->write(dataOut);
    socket->flush();
}
