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

    // изменяет данные
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // добавляет игроков
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // удаляет игроков
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // заголовки строк и столбцов
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // флаги
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // загрузка данных из файла
    void loadFromFile(QString fileName);

    // очистка данных
    void clearData();

    // рассылка
    std::optional<int> mailing();

private:
    // создание пар
    QVector<int> pairing();
};
