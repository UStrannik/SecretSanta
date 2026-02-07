#include "mainwindow.h"
#include <QtWidgets>
#include <QSslSocket>
#include <QKeyEvent>
#include <QEvent>

// создаёт тело письма
QByteArray MainWindow::makeEmailBody(QString from, QString to, QString subj, QString msg)
{
    QByteArray email;

    email.append("Subject: " + subj.toUtf8() + "\r\n"); // тема
    email.append("From: " + from.toUtf8() + "\r\n");    // отправитель
    email.append("To: " + to.toUtf8() + "\r\n");        // получатель
    email.append("\r\n");                               // пустая строка
    email.append(msg.toUtf8());                         // текст письма
    email.append("\r\n.\r\n");                          // завершение письма

    return email;
}

// отправка письма
bool MainWindow::sendSmtpEmail(QString from, QString to, QString subj, QString msg)
{
    // отправка заголовков
    sslSocket.write("MAIL FROM:<" + from.toUtf8() + ">\r\n");
    sslSocket.waitForReadyRead(10000);
    sslSocket.write("RCPT TO:<" + to.toUtf8() + ">\r\n");
    sslSocket.waitForReadyRead(10000);
    sslSocket.write("DATA\r\n");
    sslSocket.waitForReadyRead(10000);

    // отправка письма
    QByteArray eml = makeEmailBody(from, to, subj, msg);
    sslSocket.write(eml);
    sslSocket.waitForReadyRead(10000);

    // получение ответа сервера
    QString resp = QString::fromUtf8(sslSocket.readAll());

    // сброс соединения
    sslSocket.write("RSET\r\n");
    sslSocket.waitForReadyRead();

    // проверка успешности отправки (250 - успешно)
    return resp.contains("250");
}

// конструктор
MainWindow::MainWindow(DataModel *_model, QWidget *parent)
    : QMainWindow(parent)
{
    // сохраняем указатель на модель
    dataModel = _model;

    // заголовок окна
    setWindowTitle("Тайный Санта");

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
    connect(pbtnOpenFile, SIGNAL(clicked(bool)), SLOT(slotLoad()));
    pltData->addWidget(pbtnOpenFile);
    pltMain->addLayout(pltData);

    // таблица
    ptbvTable = new QTableView;
    ptbvTable->setModel(dataModel);
    pltMain->addWidget(ptbvTable);

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
    connect(pbtnTestMail, SIGNAL(clicked(bool)), SLOT(slotSendTest()));
    pbtnSendSanta = new QPushButton("Рассылка");
    connect(pbtnSendSanta, SIGNAL(clicked(bool)), SLOT(slotSendSanta()));
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
void MainWindow::slotLoad()
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
void MainWindow::slotSendTest()
{
    /* TODO
     * реализовать в классе работы с почтой
     *
    // попытка подключения к серверу
    if (!connectToSmtpServer(ptxtServer->text(), ptxtPort->text()))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось подключиться к серверу!", QMessageBox::Ok);
        closeSmtpConnection();
        return;
    }

    // попытка авторизации на сервере
    if (!loginOnSmtpServer(ptxtUser->text(), ptxtPassword->text()))
    {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль!", QMessageBox::Ok);
        closeSmtpConnection();
        return;
    }

    // попытка отправить письмо
    if (!sendSmtpEmail(ptxtUser->text(), ptxtUser->text(), "Тайный Санта", "Тестовое письмо"))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось отправить письмо!", QMessageBox::Ok);
        closeSmtpConnection();
    }

    // завершение соединения
    closeSmtpConnection();

    // уведомление об успехе
    QMessageBox::information(this, "Информация", "Тестовое письмо отправлено, проверьте почтовый ящик.", QMessageBox::Ok);

    */
}

void MainWindow::slotSendSanta()
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
        return;
    }
    if (port <= 0)
    {
        QMessageBox::critical(this, "Ошибка!", "Указан некорректный порт сервера!",
                              QMessageBox::Ok);
        return;
    }

    // проверяем указаны ли логин и пароль сервера
    if (login.isEmpty())
    {
        QMessageBox::critical(this, "Ошибка!", "Указан некорректный логин!",
                              QMessageBox::Ok);
        return;
    }
    if (password.isEmpty())
    {
        QMessageBox::critical(this, "Ошибка!", "Указан некорректный пароль!",
                              QMessageBox::Ok);
        return;
    }

    dataModel->mailing(address, port, login, password);
    /* TODO
     * вынести в отдельный класс
    QString allList = "";

    QString sender = ptxtUser->text();

    //пробуем подключиться к серверу
    if (!connectToSmtpServer(ptxtServer->text(), ptxtPort->text()))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось подключиться к серверу. Письма не отправлены!",
                             QMessageBox::Ok);
        return;
    }

    // пробуем авторизоваться на сервере
    if (!loginOnSmtpServer(ptxtUser->text(), ptxtPassword->text()))
    {
        QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль. Письма не отправлены!",
                             QMessageBox::Ok);
        return;
    }

    // если размеры списков не совпадают, то что-то не так и рассылку делать нельзя
    if (from.size() != to.size())
    {
        QMessageBox::warning(this, "Ошибка", "Ошибка обработки данных. Обратитесь к разработчику.",
                             QMessageBox::Ok);
        return;
    }

    // проход по списку и рассылка
    for (int i = 0; i < from.size(); ++i)
    {
        if (!sendSmtpEmail(sender, from[i]->email, "Secret Santa", "Your recipient: " + to[i]->name))
            QMessageBox::warning(this, "Warning",
                                 "E-Mail to " + from[i]->name + "<" + from[i]->email + ">" + " not sended!",
                                 QMessageBox::Ok);

        allList += from[i]->name + " -> " + to[i]->name + "\n";
    }

    // отправка списка
    if (!sendSmtpEmail(sender, ptxtUser->text().toUtf8(), "Secret Santa list", allList))
        QMessageBox::warning(this, "Warning", "Self-message with all-list not sended!", QMessageBox::Ok);

    closeSmtpConnection(); // завершение соединения

    pbtnSendSanta->setEnabled(false);

    */
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
