#include "datamodel.h"

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
    if (index.row() < 0 || index.row() > (rowCount() - 1))
        return QVariant();
    if (index.column() < 0 || index.column() > (columnCount() - 1))
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
