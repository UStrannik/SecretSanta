#include "mailsend.h"
#include <QThread>

MailSend::MailSend(QString address, int port, QString login, QString pass) :
    serverAddress(address), serverPort(port), serverLogin(login), serverPassword(pass)
{
    //
}

void MailSend::sendSanta(QList<QSharedPointer<Gamer> > listOfGamers)
{
    QThread::msleep(10000);
}

// подключение к серверу SMTP
bool MailSend::connectToSmtpServer()
{
    if (sslSocket.isOpen())    // закрыть сокет если открыт
        sslSocket.close();

    sslSocket.connectToHostEncrypted(serverAddress, serverPort);  // подключиться по ssl
    sslSocket.waitForConnected();   // дождаться подключения
    sslSocket.waitForEncrypted();   // дождаться рукопожатия

    return sslSocket.isEncrypted(); // вернуть результат
}

// авторизация на SMTP сервере
bool MailSend::loginOnSmtpServer()
{
    // если соединение нет, то возвращаем false
    if (!sslSocket.isEncrypted())
        return false;

    // авторизация
    sslSocket.write("EHLO myapp\r\n");
    sslSocket.waitForReadyRead();
    sslSocket.write("AUTH LOGIN\r\n");
    sslSocket.waitForReadyRead();
    sslSocket.write(serverLogin.toUtf8().toBase64() + "\r\n");
    sslSocket.waitForReadyRead();
    sslSocket.write(serverPassword.toUtf8().toBase64() + "\r\n");
    sslSocket.waitForReadyRead();

    // проверка успешности авторизации
    QString resp = QString::fromUtf8(sslSocket.readAll());
    return !resp.contains("535");
}

// завершает соединение с SMTP сервером
void MailSend::closeSmtpConnection()
{
    // если соединения уже нет, то возврат
    if (!sslSocket.isEncrypted())
        return;
    sslSocket.write("QUIT\r\n");
    sslSocket.waitForReadyRead();
    sslSocket.disconnectFromHost();
    sslSocket.waitForDisconnected();
}
