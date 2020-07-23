#ifndef _CHAT_CLIENT_HPP
#define _CHAT_CLIENT_HPP

//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "asio.hpp"
#include "chat_message.hpp"
#include "player.hpp"

using asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;
class Player_window;
class chat_client
{
public:
  chat_client(asio::io_context& io_context, const tcp::resolver::results_type& endpoints);
  void write(const chat_message& msg);
  void close();
  void set_uuid();
  void do_connect(const tcp::resolver::results_type& endpoints);
  void do_read_header();
  void do_read_body();
  void do_write();
  void update(Player_window *p);
  std::string get_players_ID();
  asio::io_context& io_context_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
  std::string uuid;
  std::mutex mtx;
};

#endif
