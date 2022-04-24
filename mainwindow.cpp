#include <QCoreApplication>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QByteArray>
#include <QDebug>
#include <QtCore>
#include <QProcess>
#include <QObject>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bot = new TelegramBot(TOKEN) ;
    arduino = new QSerialPort();
  //  answer();
    botWork();

    QSerialPortInfo info;
    if(info.availablePorts().size() > 1) {
        arduino->setPort(info.availablePorts()[1]);
    }
    arduino->setBaudRate(QSerialPort::Baud9600);
    arduino->setDataBits(QSerialPort::Data8);
    arduino->setParity(QSerialPort::NoParity);
    arduino->setStopBits(QSerialPort::OneStop);
    arduino->setFlowControl(QSerialPort::NoFlowControl);

    filew.setFileName("Readme.txt");


    bot->constructTextButton("Привет");




    if (arduino->open(QIODevice::ReadWrite)) {
        QString program = "C:\\Program Files\\TeamViewer\\TeamViewer.exe";
        QProcess *myProcess = new QProcess(this);
        myProcess->setProgram(program);
        connect(arduino, &QSerialPort::readyRead, this, [&](){

            QString s;
            QString d = arduino->readAll();

            int i = d.toInt() ;
            for(auto user : users.keys()){
                if (d == "led") {
                    bot->sendMessage(user, "есть освещение");
                }

                if( i == 1){
                    //  myProcess->start();

                    s = "нажал";
                    bot->sendMessage(user, "кнопка нажата");

                }
                else if (i == 0){
                    s = "не нажато";
                }

                if(i > 3) {
                    bot->sendMessage(user, "расстояние: " + QString::number(i) + " см");
                }
            }

        });
    }

}


MainWindow::~MainWindow()
{      
    delete ui;
}

#include <chrono>

void MainWindow::botWork()
{

    QDateTime tt;
    QObject::connect(bot, &TelegramBot::newMessage, [&](TelegramBotUpdate update) {

        TelegramBotMessage * message = update->message;

        QList<TelegramBotKeyboardButtonRequest> botButtons;

//        TelegramBotKeyboardButtonRequest but1 = TelegramBot::constructInlineButton("Привет", "", "", "", message->chat.title);
//        TelegramBotKeyboardButtonRequest but2 =  TelegramBot::constructInlineButton("Че как", "", "", "", message->chat.title);
//        TelegramBotKeyboardButtonRequest but3 =  TelegramBot::constructInlineButton("Пока", "", "", "", message->chat.title);

//        botButtons.append(but1);
//        botButtons.append(but2);
//        botButtons.append(but3);


        if (message != nullptr) {

            QString str = message->text;

            QString txt = QString("New Message from: %1 %2 (%3) -> %4").
                    arg(message->from.firstName).
                    arg(message->from.lastName).
                    arg(message->from.username).
                    arg(message->text);

            if(message->text.size()) {
                ui->textBrowser->setText(txt);
                bot->sendChatAction(message->from.id, TelegramBotChatAction::Typing);

                QTextStream ts(&filew);

                if(str.contains(":")){
                    QFile file(QString::number(message->from.id));

                    if(file.open(QIODevice::Append)){
                        QTextStream ts(&file);
                        ts << QString("\n" +str);
                        file.close();
                        answer(QString::number(message->from.id));
                    }
                }

                if(filew.open(QIODevice::Append)) {
                    ts << txt + "\n";
                    ts.flush();
                }
                filew.close();

                if(mapa.contains(str))
                    bot->sendMessage(message->from.id, mapa.value(str));

                if(message->text == "/start" ){
                    bool hasUser = true;
                    userId = message->from.id;
                    if(users.isEmpty()) {
                        users.insert(userId, message->from.username);
                        for(auto u : users.keys()) {
                            bot->sendMessage(u, "пользователь: " + users.value(u) + " добавлен");

                        }
                    }

                    for(auto user : users.keys()) {
                        if (user != userId){
                            hasUser = false;
                        }
                        if(!hasUser) {
                            bot->sendMessage(message->from.id, "Драсте");
                            users.insert(userId, message->from.username);
                            for(auto u : users.keys()) {
                                bot->sendMessage(u, "пользователь: " + message->from.username + " добавлен");
                                hasUser = true;
                            }
                        }

                    }

                }


                if(message->text == "/stop" ) {
                    for(auto user : users.keys()){
                        bot->sendMessage(user, "пользователь: " + message->from.username + " удалён");
                        qDebug() << "пользователь: " + message->from.username + " удалён";
                        users.remove(message->from.id);
                    }

                }

                arduino->write(message->text.toUtf8());
            }
        }
    });
    bot->startMessagePulling();
}

void MainWindow::answer(QString filename)
{
    QFile f(filename);
    if(f.open(QIODevice::ReadOnly)){
        QTextStream ts(&f);
        QString str = ts.readAll();
        QStringList liststr = str.split("\n");
         qDebug() << str;
        for (auto list : liststr) {
            if(list.isEmpty() || !list.contains(":"))
                continue;
            mapa.insert(list.split(":")[0].trimmed(), list.split(":")[1].trimmed());

        }
        f.close();
    }
}



void MainWindow::on_pushButton_clicked()
{
    if (arduino->isOpen()) {
        arduino->close();
        if(arduino->open(QIODevice::ReadWrite)) {
            ui->textBrowser->setText("ардуино преподключено");

        }
    } else {
        if(arduino->open(QIODevice::ReadWrite))
            ui->textBrowser->setText("подключен") ;
    }
}


void MainWindow::on_pushButton_2_clicked()
{
    if (arduino->isOpen())
        arduino->close();
}


