#pragma once

#include <QMainWindow>
#include <QtWidgets>
#include <QList>
#include <QSslSocket>
#include <delegats.h>

class SecretSanta : public QMainWindow
{
    Q_OBJECT

private:
    struct Member                   // участник
    {
        QString name;   // имя
        QString email;  // почта
    };

    QList<Member*> from;            // список участников
    QList<Member*> to;              // список пар для участников

    QLabel          *plblServer;    // метка адрес сервера
    QLabel          *plblPort;      // метка порт сервера
    QLabel          *plblUser;      // метка логин сервера
    QLabel          *plblPassword;  // метка пароль сервера

    QLineEdit       *ptxtServer;    // поле ввода адреса сервера
    QLineEdit       *ptxtPort;      // поле ввода номера порта сервера
    QLineEdit       *ptxtUser;      // поле ввода логина
    QLineEdit       *ptxtPassword;  // поле ввода пароля

    QPushButton     *pbtnOpenFile;  // кнопка загрузки из файла
    QPushButton     *pbtnTestMail;  // кнопка отправки тестового письма
    QPushButton     *pbtnSendSanta; // кнопка запуска рассылки

    QTableWidget    *ptbwTable;     // таблица со списком рассылки

    QSslSocket socket;              // сокет для подключений

    /* работа с файлами */
    bool loadFromFile(QString fileName);                                            // загрузка из файла
    bool parseString(QString const &src, QString &name, QString &eml);              // разбирает строку на имя и почту

    /* работа с данными */
    bool pairing();                 // создание пар участников

    /* работа с почтой */
    QByteArray makeEmailBody(QString from, QString to, QString subj, QString msg);  // формирует письмо
    bool connectToSmtpServer(QString addr, QString port);                           // подключается к серверу
    bool loginOnSmtpServer(QString login, QString password);                        // авторизуется на сервере
    bool sendSmtpEmail(QString from, QString to, QString subj, QString msg);        // отправляет письмо
    void closeSmtpConnection();                                                     // завершает соединение

    /* работа с интерфейсом */
    void updateTable();                                                             // обновление списка в окне
    void clearTable();                                                              // очистка списка в окне

public:
    SecretSanta(QWidget *parent = nullptr);

public slots:
    void slotLoad();
    void slotSendTest();
    void slotSendSanta();
};
