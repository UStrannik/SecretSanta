#pragma once

#include "qobject.h"
#include <QString>


// игрок
struct Gamer
{
    QString name;   // имя
    QString email;  // почта

    /*
     * названия столбцов для отображения
     * Count всегда должно быть последним - это надо
     * для определения количества столбцов
     */
    enum сolumns { NameColumn, EmailColumn, Count };

    // конструктор без аргументов
    Gamer();

    // конструктор копирования
    Gamer(const Gamer &gamer);

    // оператор присваивания
    Gamer &operator=(const Gamer &gamer);

    // деструктор
    ~Gamer();
};
