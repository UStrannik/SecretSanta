#pragma once

#include "gamer.h"
#include <QObject>
#include <QSharedPointer>
#include <QSslSocket>

class MailSend : public QObject
{
    Q_OBJECT

public:
    explicit MailSend(QString address, int port,
                      QString login, QString pass);

    void sendSanta(QList<QSharedPointer<Gamer>> listOfGamers);
    void sendTest();

private:
    QSslSocket sslSocket;                       // сокет для подключений
    QList<QSharedPointer<Gamer>> listOfGamers;  // список указателей на игроков для рассылки
    QString serverAddress;                      // адрес сервера
    int serverPort;                             // номер порта сервера
    QString serverLogin;                        // логин для авторизации на сервере
    QString serverPassword;                     // пароль на сервере


    QByteArray makeSantaEmail(QSharedPointer<Gamer> gamer); // формирует письмо для игрока
    QByteArray makeTestEmail();                             // формирует тестовое письмо
    bool connectToSmtpServer();                             // подключается к серверу
    bool loginOnSmtpServer();                               // авторизуется на сервере
    bool sendSmtpEmail(QString from, QString to,
                       QByteArray msg);                     // отправляет письмо
    void closeSmtpConnection();                             // завершает соединение

signals:
};
