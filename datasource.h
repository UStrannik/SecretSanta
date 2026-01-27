#pragma once

#include "gamer.h"
#include <QObject>

class DataSource : public QObject
{
    Q_OBJECT

public:
    explicit DataSource(QObject *parent = nullptr);
    ~DataSource();

private:

    // список игроков
    QList<QSharedPointer<Gamer>> *listOfGamers;


    // парсинг строки
    bool parseString(QString const &src, Gamer &gamer);

    // заполняет тестовыми данными для отладки
    void testData();

public:

    // загрузка из файла
    std::optional<int> loadFromFile(QString fileName);

    // возврат количества игроков
    // TODO
    qsizetype getCount();

    // возврат игрока по константной ссылке по индексу
    // TODO
    const Gamer getGamer(qsizetype index);

    // обновление данных об игроке по индексу
    // TODO
    bool updateGamer(qsizetype index, Gamer gamer);

    // удаление игрока по индексу
    // TODO
    bool deleteGamer(qsizetype index);


signals:


};
