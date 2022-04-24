#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include "QMap"
#include "telegrambot.h"

#define TOKEN ""

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void botWork();

    QSerialPort *arduino;
    quint32 userId;
    QMap<quint32, QString> users;

    QMap<QString, QString> mapa;
    TelegramBot  *bot;

    QFile filew;
    void answer(QString filename);



private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;



};
#endif // MAINWINDOW_H
