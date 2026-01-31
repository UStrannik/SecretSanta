#include "mainwindow.h"
#include <QtWidgets>
#include <QSslSocket>

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

// создание списка пар
bool MainWindow::pairing()
{
    /* TODO
     * вынести реализацию в модель
     *
    if (from.isEmpty())     // пустой список  невозможно перемешать
        return false;

    to = from;              // копируем список отправителей в список получателей
    int n = to.size();      // получаем размер списка

    if (n == 1)             // если в списке только один элемент,
        return true;        // то его не надо перемешивать

    /*
     * Идем по списку от конца к началу. На каждом шаге генерируем случайное j,
     * которое обязательно меньше i. Меняем местами элементы i и j. Получаем
     * список, в которое все элементы сменили свою позицию.
     *
    int j = 0;
    for (int i = n - 1; i > 0; --i) {
        j = QRandomGenerator::global()->bounded(i);
        to.swapItemsAt(i, j);
    }

    */

    return true;
}

// подключаемся к серверу
bool MainWindow::connectToSmtpServer(QString addr, QString port)
{
    if (socket.isOpen())    // закрыть сокет если открыт
        socket.close();

    socket.connectToHostEncrypted(addr, port.toInt());  // подключиться
    socket.waitForConnected();

    return socket.isOpen();     // вернуть результат
}

// авторизуемся на сервере
bool MainWindow::loginOnSmtpServer(QString login, QString password)
{
    // авторизация по SMTP
    socket.write("EHLO myapp\r\n");
    socket.waitForReadyRead();
    socket.write("AUTH LOGIN\r\n");
    socket.waitForReadyRead();
    socket.write(login.toUtf8().toBase64() + "\r\n");
    socket.waitForReadyRead();
    socket.write(password.toUtf8().toBase64() + "\r\n");
    socket.waitForReadyRead();

    // проверка успешности авторизации
    QString resp = QString::fromUtf8(socket.readAll());
    return !resp.contains("535");
}

// отправка письма
bool MainWindow::sendSmtpEmail(QString from, QString to, QString subj, QString msg)
{
    // отправка заголовков
    socket.write("MAIL FROM:<" + from.toUtf8() + ">\r\n");
    socket.waitForReadyRead(10000);
    socket.write("RCPT TO:<" + to.toUtf8() + ">\r\n");
    socket.waitForReadyRead(10000);
    socket.write("DATA\r\n");
    socket.waitForReadyRead(10000);

    // отправка письма
    QByteArray eml = makeEmailBody(from, to, subj, msg);
    socket.write(eml);
    socket.waitForReadyRead(10000);

    // получение ответа сервера
    QString resp = QString::fromUtf8(socket.readAll());

    // сброс соединения
    socket.write("RSET\r\n");
    socket.waitForReadyRead();

    // проверка успешности отправки (250 - успешно)
    return resp.contains("250");
}

// завершение соединения с SMTP сервером
void MainWindow::closeSmtpConnection()
{
    socket.write("QUIT\r\n");
    socket.waitForReadyRead();
    socket.close();
}


// конструктор
MainWindow::MainWindow(DataModel *_model, QWidget *parent)
    : QMainWindow(parent)
{
    // сохраняем указатель на модель
    model = _model;

    // заголовок окна
    setWindowTitle("Тайный Санта");

    // создание основного виджета
    QWidget *pwgtMain = new QWidget;
    QVBoxLayout *pltMain = new QVBoxLayout;

    // блок ввода данных
    pbtnOpenFile = new QPushButton("Загрузить список...");
    connect(pbtnOpenFile, SIGNAL(clicked(bool)), SLOT(slotLoad()));
    pltMain->addWidget(pbtnOpenFile);

    // таблица
    ptbvTable = new QTableView;
    ptbvTable->setModel(model);
    // отключить возможность редактирования элементов (делегат)
    ptbvTable->setItemDelegate(new NonEditTableDelegate());
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
    pbtnSendSanta->setEnabled(false);
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

// слот для кнопки загрузки из файла
void MainWindow::slotLoad()
{
    // TODO
    // реализовать загрузку через модель
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
