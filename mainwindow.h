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
    /* компоненты */
    DataModel       *dataModel;     // указатель на модель
    QProgressDialog *progressWindow;// указатель на окно прогресса выполнения действий

    /* виджеты */
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
    QTableView      *ptbvTable;     // таблица со списком игроков

    bool checkServer();             // проверяет ввел ли пользователь данные для подключения к серверу

public:
    MainWindow(QWidget *parent, DataModel *_model);

public slots:
    void slotClearData();           // очистка списка игроков
    void slotLoadFromFile();        // загрузщка игроков из файла
    void slotTestEmail();           // отправка тестового письма
    void slotSendSanta();           // запуск рассылки
    void slotShowProgressWindow();  // отображение окна прогресса рассылки
    void slotCloseProgressWindow(); // закрытие окна прогресса
};
