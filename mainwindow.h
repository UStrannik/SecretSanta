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
    DataModel       *dataModel;     // указатель на модель
    QProgressDialog *progressWindow;// указатель на окно прогресса выполнения действий

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

    QTableView       *ptbvTable;    // таблица со списком игроков

    QSslSocket sslSocket;              // сокет для подключений

    /* работа с почтой */
    QByteArray makeEmailBody(QString from, QString to, QString subj, QString msg);  // формирует письмо
    bool sendSmtpEmail(QString from, QString to, QString subj, QString msg);        // отправляет письмо

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
    void slotShowProgressWindow();
    void slotCloseProgressWindow();
};
