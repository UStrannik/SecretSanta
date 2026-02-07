#include "mainwindow.h"
#include <QtWidgets>
#include <QSslSocket>
#include <QKeyEvent>
#include <QEvent>

// проверка ввел ли пользователь данные сервера
bool MainWindow::checkServer()
{
    QString address = ptxtServer->text();
    int port = ptxtPort->text().toInt();
    QString login = ptxtUser->text();
    QString password = ptxtPassword->text();

    // проверяем указаны ли данные сервера
    if (address.isEmpty())
    {
        QMessageBox::critical(this, "Ошибка!", "Указан некорректный адрес сервера!",
                              QMessageBox::Ok);
        return false;
    }
    if (port <= 0)
    {
        QMessageBox::critical(this, "Ошибка!", "Указан некорректный порт сервера!",
                              QMessageBox::Ok);
        return false;
    }

    // проверяем указаны ли логин и пароль сервера
    if (login.isEmpty())
    {
        QMessageBox::critical(this, "Ошибка!", "Указан некорректный логин!",
                              QMessageBox::Ok);
        return false;
    }
    if (password.isEmpty())
    {
        QMessageBox::critical(this, "Ошибка!", "Указан некорректный пароль!",
                              QMessageBox::Ok);
        return false;
    }

    return true;
}

// конструктор
MainWindow::MainWindow(QWidget *parent, DataModel *_model)
    : QMainWindow(parent), dataModel(_model)
{
    // заголовок окна
    setWindowTitle("Тайный Санта");
    setWindowIcon(QIcon("://icons/santa.ico"));

    // создание основного виджета
    QWidget *pwgtMain = new QWidget;
    QVBoxLayout *pltMain = new QVBoxLayout;

    // блок работы с данными
    QHBoxLayout *pltData = new QHBoxLayout;
    pbtnClearData = new QPushButton("Очистить список...");
    connect(pbtnClearData, &QPushButton::clicked, this, &MainWindow::slotClearData);
    pbtnClearData->setVisible(false);   // в версии 1.0.1 не должно быть этого функционала
    pltData->addWidget(pbtnClearData);
    pbtnOpenFile = new QPushButton("Загрузить из файла...");
    connect(pbtnOpenFile, &QPushButton::clicked, this, &MainWindow::slotLoadFromFile);
    pltData->addWidget(pbtnOpenFile);
    pltMain->addLayout(pltData);

    // таблица
    ptbvTable = new QTableView;
    ptbvTable->setModel(dataModel);
    pltMain->addWidget(ptbvTable);
    ptbvTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // автоматическая ширина столбцов

    // блок информации о сервере
    QHBoxLayout *pltMailSend = new QHBoxLayout;
    QGridLayout *pltMailServer = new QGridLayout;
    plblServer = new QLabel("Сервер:");
    ptxtServer = new QLineEdit("smtp.gmail.com");
    pltMailServer->addWidget(plblServer, 0 , 0);
    pltMailServer->addWidget(ptxtServer, 0, 1);
    plblPort = new QLabel("Порт:");
    ptxtPort = new QLineEdit("465");
    pltMailServer->addWidget(plblPort, 1, 0);
    pltMailServer->addWidget(ptxtPort, 1, 1);
    plblUser = new QLabel("Логин:");
    ptxtUser = new QLineEdit;
    pltMailServer->addWidget(plblUser, 2, 0);
    pltMailServer->addWidget(ptxtUser, 2, 1);
    plblPassword = new QLabel("Пароль:");
    ptxtPassword = new QLineEdit;
    pltMailServer->addWidget(plblPassword, 3, 0);
    pltMailServer->addWidget(ptxtPassword, 3, 1);
    pltMailSend->addLayout(pltMailServer);

    // блок кнопок отправки писем
    QVBoxLayout *pltButtons = new QVBoxLayout;
    pbtnTestMail = new QPushButton("Тестовое письмо");
    connect(pbtnTestMail, &QPushButton::clicked, this, &MainWindow::slotTestEmail);
    pbtnSendSanta = new QPushButton("Рассылка");
    connect(pbtnSendSanta, &QPushButton::clicked, this, &MainWindow::slotSendSanta);
    connect(dataModel, &DataModel::beginMessaging, this, &MainWindow::slotShowProgressWindow);
    connect(dataModel, &DataModel::endMessaging, this, &MainWindow::slotCloseProgressWindow);
    //pbtnSendSanta->setEnabled(false);
    pltButtons->addStretch();
    pltButtons->addWidget(pbtnTestMail);
    pltButtons->addWidget(pbtnSendSanta);
    pltMailSend->addLayout(pltButtons);
    pltMain->addLayout(pltMailSend);
    pltMain->addStretch();

    // добавление главного виджета в окно
    pwgtMain->setLayout(pltMain);
    setCentralWidget(pwgtMain);
}

// слот очистки данных
void MainWindow::slotClearData()
{
    // вы уверены?
    auto res = QMessageBox::warning(this, "Внимание!",
                                    "Вы действительно хотите удалить все данные из списка?",
                                    QMessageBox::Yes | QMessageBox::No);

    // если пользователь отказался, то возврат
    if (res == QMessageBox::No)
        return;

    // просим модель очистить все данные
    dataModel->clearData();
}

// слот для кнопки загрузки из файла
void MainWindow::slotLoadFromFile()
{
    // показать диалог открытия файла
    QString filter = "Текстовый документ (*.txt);;Все файлы (*.*)";
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл...", "", filter);

    // если пользователь нажал Отмена, то возврат
    if (fileName.isEmpty())
        return;

    // просим модель загрузить данные из файла
    dataModel->loadFromFile(fileName);
}

// отправка тестового письма
void MainWindow::slotTestEmail()
{
    // если пользователь не ввел данные для подключения, то вернуться
    if (!checkServer())
        return;

    // получить данные для подключения
    QString address = ptxtServer->text();
    int port = ptxtPort->text().toInt();
    QString login = ptxtUser->text();
    QString password = ptxtPassword->text();

    // попросить подель запустить рассылку
    dataModel->testMessage(address, port, login, password);
}

// запуск рассылки
void MainWindow::slotSendSanta()
{
    // если пользователь не ввел данные для подключения, то вернуться
    if (!checkServer())
        return;

    // получить данные для подключения
    QString address = ptxtServer->text();
    int port = ptxtPort->text().toInt();
    QString login = ptxtUser->text();
    QString password = ptxtPassword->text();

    // попросить подель запустить рассылку
    dataModel->mailing(address, port, login, password);
}

// отобразить окно процесса
void MainWindow::slotShowProgressWindow()
{
    // создаем новое
    progressWindow = new QProgressDialog("Отправка", "Отмена", 0, 0, this);

    // делаем модальным
    progressWindow->setWindowModality(Qt::WindowModal);

    // убираем кнопку Отмена
    progressWindow->setCancelButton(nullptr);

    // убираем строку заголовка
    progressWindow->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    //TODO пофиксить возможность закрытия о кна кнопкой Esc

    // отображаем
    progressWindow->show();
}

// закрыть окно прогресса
void MainWindow::slotCloseProgressWindow()
{
    progressWindow->accept();
    delete progressWindow;
}
