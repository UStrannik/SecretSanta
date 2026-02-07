#include "datamodel.h"

#include "mailsend.h"

#include <QRandomGenerator>
#include <QtConcurrent>

// конструктор
DataModel::DataModel(DataSource *_data, QObject *parent)
    : QAbstractTableModel{parent}
{
    dataSource = _data;
}

// количество строк
int DataModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return dataSource->getCount();
}

// количество столбцов
int DataModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return Gamer::Count;
}

// возвращает запрошенные данные
QVariant DataModel::data(const QModelIndex &index, int role) const
{
    // если передан невалидный индекс,
    // то вернуть пустой объект
    if (!index.isValid())
        return QVariant();

    // если индекс за пределами диапазона,
    // то вернуть пустой объект
    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();
    if (index.column() < 0 || index.column() >= columnCount())
        return QVariant();

    // если запрошенная роль НЕ отображение и НЕ редактирование,
    // то вернуть пустой объект
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    // в зависимости от запрошенного столбца возвращаем нужное значение
    switch (index.column())
    {
        case Gamer::NameColumn : return QVariant(dataSource->getGamer(index.row())->name);
        case Gamer::EmailColumn: return QVariant(dataSource->getGamer(index.row())->email);
    }

    // если не нашлось вариантов, то возвращаем пустой объект
    return QVariant();
}

// изменяет данные
bool DataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    // если индекс не валидный, то вернуть false
    if (!index.isValid())
        return false;

    // если индекс за пределами диапазона, то вернуть false
    if (index.row() < 0 || index.row() >= rowCount())
        return false;
    if (index.column() < 0 || index.column() >= columnCount())
        return false;

    // если роль не EditRole, то вернуть false
    if (role != Qt::EditRole)
        return false;

    // получаем нужного игрока и т.к. он константный,
    // то копируем во временную структуру
    Gamer tempGamer =*(dataSource->getGamer(index.row()));

    // меняем нужное поле
    switch (index.column())
    {
    case Gamer::NameColumn :
        tempGamer.name = value.value<QString>();
        break;

    case Gamer::EmailColumn:
        tempGamer.email = value.value<QString>();
        break;
    }

    // запрашиваем изменения данных в DataSource. в случае успеха
    // высылаем сигнал, что данные изменились и возвращаем true
    if (dataSource->updateGamer(index.row(), tempGamer))
    {
        emit dataChanged(index, index);
        return true;
    }

    // если сюда дошли, то что-то не так и возвращаем false
    return false;
}

// добавляет игроков
bool DataModel::insertRows(int row, int count, const QModelIndex &parent)
{
    // если предок есть, то вернуть false
    if (parent.isValid())
        return false;

    // если позиция для вставки за пределами диапазона,
    // то вернуть false
    if (row < 0 || row > rowCount())
        return false;

    // сообщить о начале добавления
    beginInsertRows(QModelIndex(), row, row + count - 1);

    // добавить нужное количество игроков
    // TODO неплохо бы добавить проверку на успешность добавления
    for (int i = 0; i < count; i++)
    {
        Gamer gamer;
        dataSource->insertGamer(row + i, gamer);
    }

    // сообщить о завершении добавления
    endInsertRows();

    return true;
}

// удаляет игроков
bool DataModel::removeRows(int row, int count, const QModelIndex &parent)
{
    // если предок есть, то вернуть false;
    if (parent.isValid())
        return false;

    // если позиция за пределами диапазона, то вернуть false
    if (row < 0 || row >= rowCount())
        return false;

    // сообщить о начале удаления
    beginRemoveRows(QModelIndex(), row, row + count - 1);

    // удалить нужное количество игроков
    // TODO неплохо бы добавить проверку на успешность удаления
    for (int i = 0; i < count; i++)
        dataSource->deleteGamer(row + i);

    // сообщаем о завершении удаления
    endRemoveRows();

    return true;
}

// заголовки столбцов и строк
QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // если роль не DisplayRole, то вернуть пустой объект
    if (role != Qt::DisplayRole)
        return QVariant();

    // если надо заголовок строки, то вернуть "-" если
    // этому игроку не отправлялось письмо и "+" если отправлялось
    if (orientation == Qt::Vertical)
        switch (dataSource->getGamer(section)->sended)
        {
            case true: return QVariant(" + ");
            case false: return QVariant(" - ");
        }

    // определяем и возвращаем заголовок столбца
    switch (section)
    {
        case Gamer::NameColumn : return QVariant("Имя");
        case Gamer::EmailColumn: return QVariant("E-Mail");
    }

    // если не нашлось подходящего варианта,
    // то возвращаем пустой объект
    return QVariant();
}

// флаги
Qt::ItemFlags DataModel::flags(const QModelIndex &index) const
{
    // если индекс не валидный, то возвращаем пустые флаги
    if (!index.isValid())
        return Qt::NoItemFlags;

    // получаем флаги по умолчанию
    Qt::ItemFlags flg = QAbstractTableModel::flags(index);

    // добавляем возможность редактирования
    flg = flg | Qt::ItemIsEditable;

    // возвращаем итоговые флаги
    return flg;
}

// обработка запроса пользователя на загрузку из файла
void DataModel::loadFromFile(QString fileName)
{
    // предупреждаем о возможном изменении данных
    emit layoutAboutToBeChanged();

    // просим DataSource загрузить данные из выбранного пользователем файла
    // в ответ получим количество загруженных записей
    /*std::optional<int> loaded = */dataSource->loadFromFile(fileName);

    // высылаем сигнал об изменении данных
    emit layoutChanged();

    // TODO добавить сигнал с количеством загруженных данных
    // для отображения в строке состояния главного окна
}

// очистка данных
void DataModel::clearData()
{
    // предупреждаем о предстоящем изменении данных
    beginResetModel();

    // просим DataSource очистить данные
    dataSource->clearData();

    // сообщаем об окончании манипуляций
    endResetModel();
}

// рассылка
bool DataModel::mailing(QString serverAddress, int serverPort, QString serverLogin, QString serverPassword)
{
    // если игроков нет, то возвращаем пустоту
    if (dataSource->getCount() == 0)
        return false;

    // получаем копию списка всех указателей на игроков
    QList<QSharedPointer<Gamer>> listOfGamers(dataSource->getAllGamers());

    // создаем пары
    pairing(listOfGamers);

    // выносим рассылку в фоновый поток
    (void)QtConcurrent::run([=]()
                      {
                        // отправка сигнала о начале вычислений
                        emit beginMessaging(0);

                        MailSend mailSend(serverAddress, serverPort, serverLogin, serverPassword);
                        mailSend.sendSmtpSanta(listOfGamers);

                        // отправка сигнала окончания
                        emit endMessaging();
                      });

    // обновляем заголовки строк чтобы увидеть кому отправили
    emit headerDataChanged(Qt::Vertical, 0, rowCount() - 1);

    // TODO добавить очистку пар

    return true;

}

// отправка тестового сообщения
bool DataModel::testMessage(QString serverAddress, int serverPort, QString serverLogin, QString serverPassword)
{
    // отправка без создания потока т.к. отправляем одно сообщение для теста
    MailSend mailSend(serverAddress, serverPort, serverLogin, serverPassword);

    return mailSend.sendSmtpTest();
}

// создание пар
bool DataModel::pairing(QList<QSharedPointer<Gamer>> &listOfGamers)
{
    // получаем количество игроков
    int count = listOfGamers.size();

    // если список пуст, то возвращаем false
    if (count ==0)
        return false;

    // создаем вектор и заполняем индексами игроков
    QVector<int> pair(count);
    for (int i = 0; i < count; i++)
        pair[i] = i;

    // если игроков больше одного, то перемешиваем индексы
    if (count > 1)
    {
        /*
         * Идем по списку от конца к началу. На каждом шаге генерируем случайное j,
         * которое обязательно меньше i. Меняем местами элементы i и j. Получаем
         * список, в которое все элементы сменили свою позицию.
         */
        int j = 0;
        for (int i = count - 1; i > 0; --i)
        {
            j = QRandomGenerator::global()->bounded(i);
            pair.swapItemsAt(i, j);
        }
    }

    // заносим указатели на пары в список
    for (int i = 0; i < count; i++)
        listOfGamers.at(i)->mailTo = listOfGamers.at(pair[i]).data();

    return true;
}

