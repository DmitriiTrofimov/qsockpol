#ifndef SOCKPOL
#define SOCKPOL

#include <QEventLoop>
#include <QThreadPool>
#include <QTcpServer>
#include <QTcpSocket>

const QString PolicyFileRequest = "<policy-file-request/>";

const QString AllPolicy =
        "<?xml version='1.0'?>"
        "<cross-domain-policy>"
        "    <allow-access-from domain=\"*\" to-ports=\"*\" />"
        "</cross-domain-policy>";

const QString LocalPolicy =
        "<?xml version='1.0'?>"
        "<cross-domain-policy>"
        "   <allow-access-from domain=\"*\" to-ports=\"4500-4550\" />"
        "</cross-domain-policy>";

class SocketPolicyServerTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    SocketPolicyServerTask ();
    qintptr socketDescriptor() const;
    void setSocketDescriptor(const qintptr &socketDescriptor);
protected:
    void run();
public slots:
    void readyRead();
    void disconnected();
private:
    qintptr m_socketDescriptor;
    QTcpSocket *m_pTcpSocket;
    QEventLoop *m_pEventLoop;
};

class SocketPolicyServer : public QTcpServer
{
    Q_OBJECT
private:
    QThreadPool *m_pThreadPool;
public:
    SocketPolicyServer(QObject *parent = 0);
    void startServer();
protected:
    void incomingConnection(qintptr handle) override;
public slots:
    void acceptError(QAbstractSocket::SocketError socketError);
};

class SocketPolicyServerThread : public QThread
{
    Q_OBJECT
public:
    SocketPolicyServerThread(QObject *parent = 0);
private:
    SocketPolicyServer *m_pPolicyServer;
};

#endif // SOCKPOL

