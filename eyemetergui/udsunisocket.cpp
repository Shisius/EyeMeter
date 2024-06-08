#include "udsunisocket.h"
#include <QDebug>
UdsUniSocket_inner::UdsUniSocket_inner(QObject *parent)
    : QObject{parent}
{
    qDebug() << Q_FUNC_INFO;
    d_udsUniComm = new UdsUniComm(EyeMeterRoles::EYEMETER_ROLE_GUI);
    if(d_udsUniComm->start() == -1)
        d_isBound = true;
}

void UdsUniSocket_inner::receiveLoop()
{
    qDebug() << Q_FUNC_INFO;
    if(d_isBound)
    {
        while (true) {
            UdsUniPack pack;
            if(d_udsUniComm->wait_recv(pack) == -1)
                break;
            else
                emit readyRead(pack);
        }
    }
    emit finished();
}

int UdsUniSocket_inner::send(unsigned char title, unsigned char role)
{
    qDebug() << Q_FUNC_INFO;
    if(!d_isBound)
        return -1;    
    return d_udsUniComm->send(title, role);
}

bool UdsUniSocket_inner::isValid() const
{
    qDebug() << Q_FUNC_INFO;
    return d_isBound;
}


UdsUniSocket::UdsUniSocket(QObject *parent)
    : QObject{parent}
{
    qDebug() << Q_FUNC_INFO;
    d_udsUniSocket = new UdsUniSocket_inner();
    if(!d_udsUniSocket->isValid())
        return;

    d_udsUniSocket->moveToThread(&d_udsThread);
    qDebug() << connect(&d_udsThread, SIGNAL(started()), d_udsUniSocket, SLOT(receiveLoop()));
    qDebug() << connect(d_udsUniSocket, SIGNAL(readyRead(const UdsUniPack &)), SLOT(slot_readUds(const UdsUniPack &)));
    qDebug() << connect(&d_udsThread, SIGNAL(finished()), d_udsUniSocket, SLOT(deleteLater()));
    d_udsThread.start();
}

UdsUniSocket::~UdsUniSocket()
{
    qDebug() << Q_FUNC_INFO;
    d_udsThread.quit();
    d_udsThread.wait();
}

void UdsUniSocket::slot_readUds(const UdsUniPack & pack)
{
    qDebug() << Q_FUNC_INFO;
    emit readyRead(pack);
}
