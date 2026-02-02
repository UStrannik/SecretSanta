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

    // проверка правильности адреса email
    bool checkEmail(QString email);

    // заполняет тестовыми данными для отладки
    void testData();

public:

    // загрузка из файла
    std::optional<int> loadFromFile(QString fileName);

    // возврат количества игроков
    qsizetype getCount();

    // возврат игрока по константной ссылке по индексу
    QSharedPointer<const Gamer> getGamer(qsizetype index);

    // обновление данных об игроке по индексу
    bool updateGamer(qsizetype index, Gamer gamer);

    // добавляет игрока в заданную позицию
    bool insertGamer(qsizetype index, Gamer gamer);

    // удаление игрока по индексу
    bool deleteGamer(qsizetype index);

    // удаление всех игроков
    bool clearData();


signals:


};
