#pragma once
#include "clientnetwork.h"
#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

class QTextEdit;
class QLineEdit;

class ClientWindow : public QWidget {
  Q_OBJECT
private:
  const quint8 FIRST_TYPE_MSG = 0;
  const quint8 SECOND_TYPE_MSG = 1;

  QTextEdit*  out_text_;
  QLineEdit*  in_text_;
  QPushButton* in_cmd;
  QAction*   menu_action_;
  QMenuBar menu_bar_;
  QMenu* menu_;
  ClientNetwork *client_network_;

  void create_main_widget();
  void create_menu();
  void create_window_chat();
  void connect_signals();

public:
  ClientWindow(const QByteArray &user_name, const QString& str_host, int port, QWidget* pwgt = 0) ;

signals:
  void signal_send_server(QByteArray arrBlock);
  void signal_show_online();

public slots:
  void slot_ready_read(QString str);
  void slot_send_to_server();
  void slot_show_online(QString online_users);
};
