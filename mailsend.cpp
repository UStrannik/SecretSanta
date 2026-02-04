#include "mailsend.h"
#include <QThread>

MailSend::MailSend(QObject *parent)
    : QObject{parent}
{}

void MailSend::sendAll(QList<QSharedPointer<Gamer> > listOfGamers)
{
    QThread::msleep(10000);
}
