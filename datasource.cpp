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
    if (!checkEmail(_email))
        return false;

    // присваиваем данные структуре
    gamer.name = _name;
    gamer.email = _email;

    // возвращаем успех
    return true;
}

// проверка правильности email
bool DataSource::checkEmail(QString email)
{
    static const QRegularExpression validMail("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    return validMail.match(email).hasMatch();
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

// возвращает количество игроков
qsizetype DataSource::getCount()
{
    return listOfGamers->size();
}

// возвращает указатель на запрошенного по индексу игрока
QSharedPointer<const Gamer> DataSource::getGamer(qsizetype index)
{
    // вернуть указатель на запрошенного игрока
    // если индекс за пределами диапазона, то верется nullptr
    return listOfGamers->value(index);
}

bool DataSource::updateGamer(qsizetype index, Gamer gamer)
{
    // если индекс за пределами диапазона, то вернуть false
    if (index < 0 || index >= getCount())
        return false;

    // если неправильный email, то вернуть false
    if (!checkEmail(gamer.email))
        return false;

    // присвоить полученную структуру структуре,
    // на которую указывает указатель, расположенный по
    // переданному индексу
    *(listOfGamers->at(index)) = gamer;

    return true;
}

// добавляет игрока в заданную позицию
bool DataSource::insertGamer(qsizetype index, Gamer gamer)
{
    // если позиция неверна, то вернуть false
    if (index < 0 || index > getCount())
        return false;

    // если надо добавить в конец, до добавляем и возвращаем true
    if (index == getCount())
    {
        listOfGamers->append(QSharedPointer<Gamer>::create(gamer));
        return true;
    }

    // добавляем в заданную позицию
    listOfGamers->insert(index, QSharedPointer<Gamer>::create(gamer));

    return true;
}

// удаление игрока
bool DataSource::deleteGamer(qsizetype index)
{
    // если индекс за пределами диапазона, то вернуть false
    if (index < 0 || index >= getCount())
        return false;

    // удалить игрока
    listOfGamers->remove(index);

    return true;
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
