#include "sockpol.h"

#include <QMessageBox>

SocketPolicyServer::SocketPolicyServer(QObject *parent)
    : QTcpServer(parent)
{
    m_pThreadPool = new QThreadPool(this);
    m_pThreadPool->setMaxThreadCount(4);
}

void SocketPolicyServer::startServer()
{
    if (!listen(QHostAddress::Any, 843)) {
        QMessageBox::critical(0, "Server Error", QString("Unable to start the server: %1").arg(errorString()));
        close();
        return;
    }
    connect(this, SIGNAL(acceptError(QAbstractSocket::SocketError)),
            this, SLOT(acceptError(QAbstractSocket::SocketError)));
    qDebug() << "Socket policy server has started";
}

void SocketPolicyServer::incomingConnection(qintptr handle)
{
    qDebug() << "Incoming connection";

    SocketPolicyServerTask *task = new SocketPolicyServerTask();
    task->setAutoDelete(true);
    task->setSocketDescriptor(handle);

    m_pThreadPool->start(task);
}

void SocketPolicyServer::acceptError(QAbstractSocket::SocketError socketError)
{
    qDebug() << socketError;
}

SocketPolicyServerTask::SocketPolicyServerTask()
{

}

qintptr SocketPolicyServerTask::socketDescriptor() const
{
    return m_socketDescriptor;
}

void SocketPolicyServerTask::setSocketDescriptor(const qintptr &socketDescriptor)
{
    m_socketDescriptor = socketDescriptor;
}

void SocketPolicyServerTask::run()
{
    if(!m_socketDescriptor)
        return;

    m_pEventLoop = new QEventLoop();

    m_pTcpSocket = new QTcpSocket();
    m_pTcpSocket->setSocketDescriptor(m_socketDescriptor);

    connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(m_pTcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    m_pEventLoop->exec();
    delete m_pEventLoop;
}

void SocketPolicyServerTask::readyRead()
{
    QByteArray request = PolicyFileRequest.toUtf8();
    QByteArray data = m_pTcpSocket->readAll();

    for (int i = 0; i < request.count(); i++)
        if (request[i] != data[i]) {
            m_pTcpSocket->close();
            return;
        }

    if (qstrlen(request) == qstrlen(data)) {
        qDebug() << "Got policy request, sending response";
        QByteArray allPolicy = AllPolicy.toUtf8();
        m_pTcpSocket->write(allPolicy);
        m_pTcpSocket->flush();
    }
}

void SocketPolicyServerTask::disconnected()
{
    m_pTcpSocket->deleteLater();
    m_pEventLoop->exit();
}


SocketPolicyServerThread::SocketPolicyServerThread(QObject *parent) : QThread(parent)
{
    m_pPolicyServer = new SocketPolicyServer();
    m_pPolicyServer->startServer();

    QObject::connect(this, SIGNAL(finished()), m_pPolicyServer, SLOT(deleteLater()));
    QObject::connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));

    m_pPolicyServer->moveToThread(this);

    this->start();
}
