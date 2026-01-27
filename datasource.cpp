#include "datasource.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

// конструктор
DataSource::DataSource(QObject *parent)
    : QObject{parent}
{
    //создаем список игроков
    listOfGamers = new QList<QSharedPointer<Gamer>>;

    // загрузка тестовых данных
    testData();
}

// деструктор
DataSource::~DataSource()
{
    // очищаем список
    // объекты удалятся сами т.к. это QSharedPointer
    listOfGamers->clear();

    // удаляем список
    delete listOfGamers;
}

// парсер строк формата FirstName LastName <email@server.domain>
bool DataSource::parseString(const QString &src, Gamer &gamer)
{
    // пробуем разобрать строку
    // если не смогли, то возвращаем false
    static const QRegularExpression nameMail("^(.+?)\\s*<(.+?)>$");
    QRegularExpressionMatch match = nameMail.match(src);
    if (!match.hasMatch())
        return false;

    // получаем найденные данные
    QString _name = match.captured(1).trimmed();
    QString _email = match.captured(2).trimmed();

    // проверяем корректность адреса почты
    // если это не почта, то возвращаем false
    static const QRegularExpression validMail("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!validMail.match(_email).hasMatch())
        return false;

    // присваиваем данные структуре
    gamer.name = _name;
    gamer.email = _email;

    // возвращаем успех
    return true;
}

// загрузка из файла
std::optional<int> DataSource::loadFromFile(QString fileName)
{
    // попытка открыть файл
    // в случае неудачи - пустой возврат
    QFile file = QFile(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return std::nullopt;

    QTextStream txt = QTextStream(&file);   // текстовый поток
    int counter = 0;                        // счетчик загруженных игроков

    // проходим по файлу
    while(!txt.atEnd())
    {
        // читаем строку
        QString dataLine = txt.readLine();

        // если строка пустая, то пропускаем итерацию
        if (dataLine.isEmpty())
            continue;

        // временная структура в стэке
        Gamer tempGamer;

        // парсим строку. в случае успеха создаем в куче новую структуру из временной
        // и добавляем указатель на нее в listOfGamrs, а также увеличиваем счетчик
        // прочитанных данных
        if (parseString(dataLine, tempGamer))
        {
            listOfGamers->append(QSharedPointer<Gamer>::create(tempGamer));
            counter++;
        }
    }

    // закрываем файл
    file.close();

    // возвращаем количество прочитанных записей
    return counter;
}

// для тестирования работы
void DataSource::testData()
{
    /* загрузка из тестового файла */
    qDebug() << "Загружено "
             << loadFromFile("list.txt")
             << " записей.";
    /**/
}
