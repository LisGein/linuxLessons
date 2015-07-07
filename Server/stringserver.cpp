#include "stringserver.h"

StringServer::StringServer(int port, QWidget *parent)
    :tcp_server_(new QTcpServer(this))
{
    if (!tcp_server_->listen(QHostAddress::Any, port))
    {
        tcp_server_->close();
        return;
    }
    connect(tcp_server_, SIGNAL(newConnection()), this, SLOT(new_connect()));
}

StringServer::~StringServer()
{

}

void StringServer::disconnect_user()
{
    QTcpSocket* tcp_socket;
    tcp_socket = static_cast<QTcpSocket*>(sender());
    tcp_socket->deleteLater();
    int i = users_.indexOf(tcp_socket);
    last_msg_.remove(tcp_socket);
    users_.remove(i);
}

void StringServer::new_connect()
{
    while (tcp_server_->hasPendingConnections())
    {
        QTcpSocket* tcp_socket = tcp_server_->nextPendingConnection();
        connect(tcp_socket, SIGNAL(readyRead()), SLOT(read()));
        connect(tcp_socket, SIGNAL(disconnected()), SLOT(disconnect_user()));
        users_.push_back(tcp_socket);
        last_msg_.insert(tcp_socket, "");
    }
}

void StringServer::send_all(QByteArray msg)
{
    for (int i = 0; i < users_.size(); ++i)
    {
        QTextStream stream(users_[i]);
        stream.setCodec("UTF-8");
        QByteArray in_data;
        in_data.append(msg);
        stream << in_data;
    }
}

void StringServer::read()
{
    QTcpSocket* tcp_socket;
    tcp_socket = static_cast<QTcpSocket*>(sender());
    while (tcp_socket->bytesAvailable() > 0)
    {
        auto it = last_msg_.find(tcp_socket);
        QByteArray text = tcp_socket->readLine();
        QByteArray old_msg = it.value() + text;
        it.value() = "";
        if (old_msg[old_msg.size() -1] == '\n')
            emit ready_msg(old_msg);
        else
            last_msg_.insert(tcp_socket, old_msg);

    }
}