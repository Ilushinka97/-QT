#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDebug>
#include <QByteArray>
#include <QObject>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTcpSocket* socket; //Сокет для передачи информации
    QByteArray dataIn; //Массив для получения информации

public slots:
    void readyRead();
    void disconnected();
    void slotError(QAbstractSocket::SocketError err);
    void parsing();
    QByteArray Structure(char p);// Сборка пакета данных для передачи

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif
// MAINWINDOW_H

