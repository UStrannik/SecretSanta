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


    // рассылка игрокам через SMTP
    void sendSmtpSanta(QList<QSharedPointer<Gamer>> _listOfGamers);

    // отправка тестового письма через SMTP
    bool sendSmtpTest();

private:
    QList<QSharedPointer<Gamer>> listOfGamers;  // список указателей на игроков для рассылки

    /* сеть */
    QSslSocket sslSocket;                       // сокет для подключений по SSL
    QString serverAddress;                      // адрес сервера
    int serverPort;                             // номер порта сервера
    QString serverLogin;                        // логин для авторизации на сервере
    QString serverPassword;                     // пароль на сервере

    /* SMTP */
    bool connectToSmtpServer();                             // подключается к серверу
    bool loginOnSmtpServer();                               // авторизуется на сервере
    bool sendSmtpData(QString to, QByteArray msg);         // отправляет письмо через установленное подключение
    void closeSmtpConnection();                             // завершает соединение

    /* письма */
    QByteArray encodeHeader(const QString str);             // кодирует заголовки под стандарт RFC2 047
    QByteArray makeSantaEmail(QSharedPointer<Gamer> gamer); // формирует письмо для игрока
    QByteArray makeListEmail();                             // формирует письмо со списком пар для организатора
    QByteArray makeTestEmail();                             // формирует тестовое письмо

    /* отправка писем */
    bool sendForGamers(int n);          // отправляет письма игрокам пачками по n штук
    bool sendSmtpMsg(QString to,
                     QByteArray msg);   // подключается и отправляет одиночное письмо

signals:
};
