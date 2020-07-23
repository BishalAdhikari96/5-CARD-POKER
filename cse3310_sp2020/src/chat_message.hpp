//
// chat_message.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cstring>

//this class to send the entire structure of the game to the players as part of header.
class game_state
{
public:
////////////////////////////////////////////////////////////
//                                                        //
//uuid tells whose turn it is in the game using uuid      //
//balance tell what the balance of the player is          //
//bet tell what is the minimum bet in the game right now  //
//                                                        //
////////////////////////////////////////////////////////////
  char player_turn_uuid[25];
  int player_turn_balance;
  int min_bet_required;
  int min_pit_bet;
  bool has_started = false;
  int num_of_players=0;
};

//this class is to send information from a player to the server
class client_info
{
public:
///////////////////////////////////////////////////////
//                                                   //
//if the player folds, fold shall be true            //
//if the player check, check shall be true           //
//if the player goes allin, allin shall be true      //
//if the player bets, bet shall be true and the bet  //
//   amount shall be checked                         //
///////////////////////////////////////////////////////                              
  bool fold;
  bool check;
  bool swap;
  bool allin;
  bool bet;
  int bet_amount;
};

class chat_message
{
public:
  enum { header_length = 4 };//+ sizeof(client_info) + sizeof(game_state) };
  // CSE3310 maximum size of chat message body is defined
  enum { max_body_length = 4096 }; 

  chat_message()
    : body_length_(0)
  {
  }

  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  std::size_t length() const
  {
    return header_length + body_length_;
  }

  const char* body() const
  {
    return data_ + header_length;
  }

  char* body()
  {
    return data_ + header_length;
  }

  std::size_t body_length() const
  {
    return body_length_;
  }

  void body_length(std::size_t new_length)
  {
    body_length_ = new_length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
  }

  bool decode_header()
  {
    char header[header_length + 1] = "";
    std::strncat(header, data_, 4);
    body_length_ = std::atoi(header);
    char *p = data_ + 4; //skip the integer
    std::memcpy(&ca,p,sizeof(client_info));
    std::memcpy(&gs,p+sizeof(client_info),sizeof(game_state));
    
    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;
  }

  void encode_header()
  {
    char header[4 + 1] = "";
    std::sprintf(header, "%4d", static_cast<int>(body_length_));
    std::memcpy(data_, header, 4);
  }

  client_info ca;
  game_state gs;
private:
  char data_[header_length + max_body_length];
  std::size_t body_length_;
};

#endif // CHAT_MESSAGE_HPP
