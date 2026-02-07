#include "mailsend.h"
#include <QThread>

MailSend::MailSend(QString address, int port, QString login, QString pass) :
    serverAddress(address), serverPort(port), serverLogin(login), serverPassword(pass)
{
    //
}

void MailSend::sendSanta(QList<QSharedPointer<Gamer> > _listOfGamers)
{
    // перемещаем полученный список игроков
    listOfGamers = std::move(_listOfGamers);

    // будем отправлять письма пачками по n штук в одном соединении
    // в будущем сделать настройку этого параметра
    int n = 5;
    for (int i = 0; i < listOfGamers.size(); i += n)
    {
        // подключение к серверу
        connectToSmtpServer();

        // авторизация на сервере
        loginOnSmtpServer();

        // отправка пачки писем
        for (int j = i; (j < i + n) && (j < listOfGamers.size()); j++)
            sendSmtpEmail(listOfGamers.at(j)->mailTo->email,
                          makeSantaEmail(listOfGamers.at(j)));

        // отключение от сервера
        closeSmtpConnection();
    }

    // debug !!!
    //QThread::msleep(10000);
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

// отправляет письмо по SMTP
bool MailSend::sendSmtpEmail(QString to, QByteArray email)
{
    // отправка заголовков
    sslSocket.write("MAIL FROM:<" + serverLogin.toUtf8() + ">\r\n");
    sslSocket.waitForReadyRead(10000);
    sslSocket.write("RCPT TO:<" + to.toUtf8() + ">\r\n");
    sslSocket.waitForReadyRead(10000);
    sslSocket.write("DATA\r\n");
    sslSocket.waitForReadyRead(10000);

    // отправка письма
    sslSocket.write(email);
    sslSocket.waitForReadyRead(10000);

    // получение ответа сервера
    QString resp = QString::fromUtf8(sslSocket.readAll());

    // сброс соединения
    sslSocket.write("RSET\r\n");
    sslSocket.waitForReadyRead();

    // проверка успешности отправки (250 - успешно)
    return resp.contains("250");
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

// кодировка заголовков в формат RFC 2047
// необходима для кириллицы в заголовках
QByteArray MailSend::encodeHeader(const QString str)
{
    if (str.isEmpty()) {
        return "";
    }
    // кодируем в UTF-8, затем в Base64 и оборачиваем в спец-символы
    return "=?UTF-8?B?" + str.toUtf8().toBase64() + "?=";
}

// создает письмо для санты
QByteArray MailSend::makeSantaEmail(QSharedPointer<Gamer> gamer)
{
    QByteArray email;   // письмо
    QString msg;        // текст письма

    msg = QString("Вы должны подготовить подарок для ") + gamer->mailTo->name +
          QString("!");

    // формируем From
    email.append("From: ");
    email.append(encodeHeader("Организатор игры"));
    email.append(" <" + serverLogin.toUtf8() + ">\r\n");

    // формируем To
    email.append("To: ");
    email.append(encodeHeader(gamer->name));
    email.append(" <" + gamer->email.toUtf8() + ">\r\n");

    // формируем Subject
    email.append("Subject: ");
    email.append(encodeHeader("Тайный Санта"));
    email.append("\r\n");

    // добавляем служебные заголовки
    email.append("MIME-Version: 1.0\r\n");
    email.append("Content-Type: text/plain; charset=UTF-8\r\n");
    email.append("Content-Transfer-Encoding: 8bit\r\n");

    // добавляем пустую строку-разделитель
    email.append("\r\n");

    // добавляем текст письма
    email.append(msg.toUtf8());

    // добавляем финальную точку
    email.append("\r\n.\r\n");

    return email;
}
