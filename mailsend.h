#pragma once

#include "gamer.h"
#include <QObject>

class MailSend : public QObject
{
    Q_OBJECT
public:
    explicit MailSend(QObject *parent = nullptr);

    void sendAll(QList<QSharedPointer<Gamer>> listOfGamers);

signals:
};
