#ifndef UDSUNISOCKET_H
#define UDSUNISOCKET_H

#include <QObject>
#include "udsunicomm.h"
class UdsUniSocket_inner : public QObject
{
    Q_OBJECT
    UdsUniComm *d_udsUniComm = nullptr;
    bool d_isBound = false;
public:
    explicit UdsUniSocket_inner(QObject *parent = nullptr);
    bool isValid() const;
    int send(unsigned char title, unsigned char role = EYEMETER_ROLE_ALL);
    template<typename T>
    int send(unsigned char title, T data, unsigned char role = EYEMETER_ROLE_ALL)
    {
        if(!d_isBound)
            return -1;
        return d_udsUniComm->send(title, data, role);
    }
    void stop();

public slots:
    void receiveLoop();
signals:
    void readyRead(const UdsUniPack &);
    void finished();
};

#include <QThread>
class UdsUniSocket : public QObject
{
    Q_OBJECT
    UdsUniSocket_inner *d_udsUniSocket = nullptr;
    QThread d_udsThread;
public:
    explicit UdsUniSocket(QObject *parent = nullptr);
    ~UdsUniSocket();
    bool isValid() const
    {
        return d_udsUniSocket->isValid();
    }
    int send(unsigned char title, unsigned char role = EYEMETER_ROLE_ALL)
    {
        return d_udsUniSocket->send(title, role);
    }
    template<typename T>
    int send(unsigned char title, T data, unsigned char role = EYEMETER_ROLE_ALL)
    {
        return d_udsUniSocket->send(title, data, role);
    }
    void stop()
    {
        d_udsUniSocket->stop();
    }
public slots:
    void slot_readUds(const UdsUniPack &);

signals:
    void readyRead(const UdsUniPack &);
};

#endif // UDSUNISOCKET_H
