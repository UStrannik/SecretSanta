#include "gamer.h"

// конструктор без аргументов
Gamer::Gamer()
{
    email = name = "";
}

/*
 * оператор копирования, конструктор копирования и деструктор
 * реализованы на случай если в будущем понадобится
 * правильно копировать что-то динамическое
 */

Gamer::Gamer(const Gamer &gamer)
{
    this->email = gamer.email;
    this->name = gamer.name;
}

Gamer &Gamer::operator=(const Gamer &gamer)
{
    if (this == &gamer)
        return *this;

    this->email = gamer.email;
    this->name = gamer.name;

    return *this;
}

Gamer::~Gamer()
{
    //
}
