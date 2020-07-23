//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono> 
#include <ctime> 
#include <cstring>
#include <gtkmm.h>
#include <assert.h>
#include "json.hpp"
#include "asio.hpp"
#include "chat_client.hpp"
#include "chat_message.hpp"
//#include "player.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <X11/Xlib.h>

using asio::ip::tcp;
using namespace std;

using json = nlohmann::json;
typedef std::deque<chat_message> chat_message_queue;

////////////////////////////////////////////////////
// Global variables                               //
////////////////////////////////////////////////////
class Person *a;
class Player_window;
Player_window *g;
  chat_client::chat_client(asio::io_context& io_context,
      const tcp::resolver::results_type& endpoints)
    : io_context_(io_context),
      socket_(io_context)
  {
    do_connect(endpoints);
  }
// CSE3310 message is sent to the chat server in this function
  void chat_client::write(const chat_message& msg)
  {
    asio::post(io_context_,
        [this, msg]() 			
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
            do_write();
          }
        });
  }

  void chat_client::close()
  {
    asio::post(io_context_, [this]() { socket_.close(); });
  }

// CSE3310 connection is being established with the server
  void chat_client::do_connect(const tcp::resolver::results_type& endpoints) 
  {
    asio::async_connect(socket_, endpoints,
        [this](std::error_code ec, tcp::endpoint)
        {
          if (!ec)
          {
            do_read_header();
          }
        });
  }
 
  void chat_client::do_read_header()
  {
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), chat_message::header_length),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }
  // CSE3310 message body is being received from the server
  void chat_client::do_read_body() 
  {
    asio::async_read(socket_,
        asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](std::error_code ec, std::size_t /*length*/) 
        {
          if (!ec)
          {
            gdk_threads_enter();
            std::string rec_msg(read_msg_.body());
            rec_msg = rec_msg.substr(0,read_msg_.body_length());
            
            json from_dealer = json::parse(rec_msg);
            
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  If the player has just joined or someone has joined the game,       //
//  updating the GUI window as per the existing players and their       // 
//    balances                                                          //
//////////////////////////////////////////////////////////////////////////          
            
            if(from_dealer["signal"] == "joined")
            {
             /////////////////////////////////////////////////////////////
             // Check if the player is the one joining the game.        //
             // If yes, updating the initial gui with the existing      //
             // players in the game                                     // 
             /////////////////////////////////////////////////////////////
             
             std::cout<<"Inside the joined\n";
             if(from_dealer["uuid"] == uuid)
             { 
                int num_players = from_dealer["num_players"];
                for(int i=0;i<num_players;i++)
                {
                  
                  mtx.lock();
                  std::string name = from_dealer["player"][i]["name"];
                  g->players_names[i]->set_text(name);
                  mtx.unlock();
                  
                  mtx.lock();
                  int balance = from_dealer["player"][i]["balance"];
                  g->players_balances[i]->set_text(std::to_string(balance));
                  mtx.unlock();
                  
                  
                }
                
              }
              /////////////////////////////////////////////////////////////
              // if the player is not the one joining the game.          //
              // updating the initial gui with the existing              //
              // players in the game                                     // 
              /////////////////////////////////////////////////////////////
              
              else{
                int num_players = from_dealer["num_players"];
                for(int i=0;i<num_players;i++)
                {
                  
                  mtx.lock();
                  std::string name = from_dealer["player"][i]["name"];
                  g->players_names[i]->set_text(name);
                  mtx.unlock();
                  
                  mtx.lock();
                  int balance = from_dealer["player"][i]["balance"];
                  g->players_balances[i]->set_text(std::to_string(balance));
                  mtx.unlock();
                 }
              }
            }
//////////////////////////////////////////////////////////////////////////
// if a player starts the game                                          //
//////////////////////////////////////////////////////////////////////////        
            else if(from_dealer["signal"]=="start_game")
            {
            /////////////////////////////////////////////////////////
            //updating the cards received by the players           //
            /////////////////////////////////////////////////////////
              int num_players = from_dealer["num_players"];
              int pot = from_dealer["pot"];
              int min_bet = from_dealer["Min bet"];
              
              g->p->bet_amount = min_bet;
              
              for(int i=0;i<num_players;i++)
              {
                std::string player_id = from_dealer["player"][i]["uuid"];
                if( player_id.compare(uuid)==0)
                {
                  mtx.lock();
                  int me_bal = from_dealer["player"][i]["balance"];
                  g->p->balance = me_bal;
                  g->me_balance.set_text(std::to_string(me_bal));
                  mtx.unlock();
                  for(int j = 0 ; j<5;j++)
                {
                  int cardnum = from_dealer["player"][i]["card"][j]["number"];
                  int cardsuit = from_dealer["player"][i]["card"][j]["suit"];
                  std::string cardname = "./src/Card_Images/"+std::to_string(cardnum)+std::to_string(cardsuit)+".png";
                  
                  mtx.lock();
                  g->player_hand_img[j]->set(cardname);
                  mtx.unlock();
                }
                }
              }
              mtx.lock();
              g->start_game.set_sensitive(FALSE);
              mtx.unlock();
              ////////////////////////////////////////////////////////
              // check if it's the players turn to play             //
              ////////////////////////////////////////////////////////
              std::string player_id = from_dealer["uuid"];
              if( player_id.compare(uuid) == 0 )
              { 
                mtx.lock();
                g->button1.set_sensitive(TRUE);
                g->button5.set_sensitive(TRUE);
                g->button10.set_sensitive(TRUE);
                g->button20.set_sensitive(TRUE);
                g->button_fold.set_sensitive(TRUE);
                g->button_check.set_sensitive(TRUE);
                g->button_swap.set_sensitive(TRUE);
                g->button_allin.set_sensitive(TRUE);
                g->button_bet.set_sensitive(TRUE);
                g->turn_label.set_text("Your Turn");
                g->pot.set_text(std::to_string(pot));
                g->bet.set_text(std::to_string(min_bet));
                mtx.unlock();
                
                mtx.lock();
                int me_bal = from_dealer["balance"];
                g->p->balance = me_bal;
                g->me_balance.set_text(std::to_string(me_bal));
                mtx.unlock();
                
                int num_players = from_dealer["num_players"];
                for(int i=0;i<num_players;i++)
                {
                  
                  mtx.lock();
                  std::string name = from_dealer["player"][i]["name"];
                  g->players_names[i]->set_text(name);
                  mtx.unlock();
                  
                  mtx.lock();
                  int balance = from_dealer["player"][i]["balance"];
                  g->players_balances[i]->set_text(std::to_string(balance));
                  mtx.unlock();
                 }
              }
              else
              {
                std::string turn = from_dealer["name"];
                mtx.lock();
                g->turn_label.set_text(turn);
                g->pot.set_text(std::to_string(pot));
                g->bet.set_text(std::to_string(min_bet));
                mtx.unlock();
                
                int num_players = from_dealer["num_players"];
                for(int i=0;i<num_players;i++)
                {
                  
                  mtx.lock();
                  std::string name = from_dealer["player"][i]["name"];
                  g->players_names[i]->set_text(name);
                  mtx.unlock();
                  
                  mtx.lock();
                  int balance = from_dealer["player"][i]["balance"];
                  g->players_balances[i]->set_text(std::to_string(balance));
                  mtx.unlock();
                 }
              }
            }
//////////////////////////////////////////////////////////////////////////
// if bet has been made                                                 //
//////////////////////////////////////////////////////////////////////////             
            else if(from_dealer["signal"]=="bet_made")
            {
              ////////////////////////////////////////////////////////
              // update all the player info                         //
              ////////////////////////////////////////////////////////
              int num_players = from_dealer["num_players"];
                for(int i=0;i<num_players;i++)
                {
                  
                  mtx.lock();
                  std::string name = from_dealer["player"][i]["name"];
                  g->players_names[i]->set_text(name);
                  mtx.unlock();
                  
                  mtx.lock();
                  int balance = from_dealer["player"][i]["balance"];
                  g->players_balances[i]->set_text(std::to_string(balance));
                  mtx.unlock();
                 }
              ////////////////////////////////////////////////////////
              // update pot, player turn and min_bet                //
              ////////////////////////////////////////////////////////
              
              int pot = from_dealer["pot"];
              int min_bet = from_dealer["Min bet"];
              g->p->bet_amount = min_bet;
              mtx.lock();
              g->pot.set_text(std::to_string(pot));
              g->bet.set_text(std::to_string(min_bet));
              mtx.unlock();
              
              ////////////////////////////////////////////////////////
              // checking if this player is the next player         //
              ////////////////////////////////////////////////////////
              std::string player_id = from_dealer["uuid"];
              if( player_id.compare(uuid) == 0 )
              { 
                mtx.lock();
                g->button1.set_sensitive(TRUE);
                g->button5.set_sensitive(TRUE);
                g->button10.set_sensitive(TRUE);
                g->button20.set_sensitive(TRUE);
                g->button_fold.set_sensitive(TRUE);
                g->button_check.set_sensitive(TRUE);
                g->button_swap.set_sensitive(TRUE);
                g->button_allin.set_sensitive(TRUE);
                g->button_bet.set_sensitive(TRUE);
                g->turn_label.set_text("Your Turn");
                g->pot.set_text(std::to_string(pot));
                g->bet.set_text(std::to_string(min_bet));
                mtx.unlock();
              }
              ////////////////////////////////////////////////////////
              // checking if this player is not the next player     //
              // updating the info                                  //  
              ////////////////////////////////////////////////////////
              else
              {
                std::string turn = from_dealer["name"];
                
                mtx.lock();
                g->button1.set_sensitive(FALSE);
                g->button5.set_sensitive(FALSE);
                g->button10.set_sensitive(FALSE);
                g->button20.set_sensitive(FALSE);
                g->button_fold.set_sensitive(FALSE);
                g->button_check.set_sensitive(FALSE);
                g->button_swap.set_sensitive(FALSE);
                g->button_allin.set_sensitive(FALSE);
                g->button_bet.set_sensitive(FALSE);
                g->turn_label.set_text(turn);
                g->pot.set_text(std::to_string(pot));
                g->bet.set_text(std::to_string(min_bet));
                mtx.unlock();
              }
            }
             gdk_threads_leave();
             do_read_header();
          }
         
          else
          {
            socket_.close();
          }
        });
  }

  void chat_client::do_write()
  {
    asio::async_write(socket_,
        asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            socket_.close();
          }
        });
  }
  void chat_client::set_uuid()
  {
    boost::uuids::basic_random_generator<boost::mt19937> g;
    boost::uuids::uuid u = g();
    string ID;
    stringstream ss;
    ss<<u;
    ID = ss.str();
    this->uuid = ID;
  }
  std::string chat_client::get_players_ID()
  {
    return uuid; 
  }
  void chat_client::update(Player_window *s) 
  {
    g=s;
  }
int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    chat_client c(io_context, endpoints);
    c.set_uuid();
    std::thread t([&io_context](){ io_context.run(); });
    XInitThreads();
    
    //Starting GUI
    Gtk::Main app(argc, argv);
    
    Person p;
    Start_menu w(&p,&c);

    Gtk::Main::run(w);
    c.close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
