#pragma once

#include "datamodel.h"

#include <QMainWindow>
#include <QtWidgets>
#include <QList>
#include <QSslSocket>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    DataModel       *dataModel;         // указатель на модель

    QLabel          *plblServer;    // метка адрес сервера
    QLabel          *plblPort;      // метка порт сервера
    QLabel          *plblUser;      // метка логин сервера
    QLabel          *plblPassword;  // метка пароль сервера

    QLineEdit       *ptxtServer;    // поле ввода адреса сервера
    QLineEdit       *ptxtPort;      // поле ввода номера порта сервера
    QLineEdit       *ptxtUser;      // поле ввода логина
    QLineEdit       *ptxtPassword;  // поле ввода пароля

    QPushButton     *pbtnClearData; // кнопка очистки данных
    QPushButton     *pbtnOpenFile;  // кнопка загрузки из файла
    QPushButton     *pbtnTestMail;  // кнопка отправки тестового письма
    QPushButton     *pbtnSendSanta; // кнопка запуска рассылки

    QTableView       *ptbvTable;     // таблица со списком рассылки

    QSslSocket socket;              // сокет для подключений

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
    MainWindow(DataModel *_model, QWidget *parent = nullptr);

public slots:
    void slotClearData();
    void slotLoad();
    void slotSendTest();
    void slotSendSanta();
};
