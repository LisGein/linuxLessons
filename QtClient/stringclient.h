#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QTextStream>
#include "rapidjson/document.h"

class StringClient : public QObject {
   Q_OBJECT
public:
   explicit StringClient(int port);
   ~StringClient();
signals:
   void ready_msg(QString const &message);

private slots:
   void send(QString msg);
   void read();

private:
   QString last_msg_;

   QTcpSocket* tcp_socket_;
};
