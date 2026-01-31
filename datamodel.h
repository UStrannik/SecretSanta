#pragma once

#include "datasource.h"

#include <QAbstractTableModel>

class DataModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    // указатель на DataSource
    DataSource *dataSource;

public:
    // конструкторы
    explicit DataModel(DataSource *_data, QObject *parent = nullptr);

    // количество строк
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    // количество столбцов
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // данные по индексу
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;


};
