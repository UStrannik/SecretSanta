#pragma once

#include "qobject.h"
#include <QString>


// игрок
struct Gamer
{
    QString name;   // имя
    QString email;  // почта

    // конструктор без аргументов
    Gamer();

    // конструктор копирования
    Gamer(Gamer &gamer);

    // оператор присваивания
    Gamer &operator=(const Gamer &gamer);

    // деструктор
    ~Gamer();
};
