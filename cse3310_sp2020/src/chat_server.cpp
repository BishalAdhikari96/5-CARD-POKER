//
// chat_server.cpp
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
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <string>
#include <cstring>
#include "json.hpp"
#include "asio.hpp"
#include "chat_message.hpp"

using asio::ip::tcp;
using json = nlohmann::json;
//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

using namespace std;
  enum card_number {ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING};
  enum suit {DIAMONDS=1, HEARTS, SPADES, CLUBS};
class Cards
{
public:  
  card_number cardNumber;
  suit cardSuit;
  Cards(card_number c, suit s):cardNumber(c), cardSuit(s)
  {
  }
  //function to return the value of card number value of a card
  int getCardNumber()
  {
    return cardNumber;
  }
  // function to return the suit of the card
  int getCardSuit()
  {
    return cardSuit;
  }

};

class Cards_Deck
{
public:
  vector<Cards> deck;
  //vector<Cards> hands;
  Cards_Deck()
  {  
    for(unsigned int i = DIAMONDS; i<=CLUBS; i++)
    {
      for(unsigned int j = ACE; j<=KING; j++)
      {
        Cards cardsHere(static_cast<card_number>(j),static_cast<suit>(i));
        deck.push_back(cardsHere);
      }
    }
  }
  //function to shuffle the cards
  void shuffle_deck()
  {
	srand(time(NULL));
	unsigned int randnum = rand() % 100+1;
	unsigned int randnum2 = rand() % 50+1;
	for(unsigned int i = 0; i<(randnum*randnum2);i++)
		random_shuffle(deck.begin(),deck.end());
  }
};

class Hands
{
	public:
		vector<Cards> allhands;
		vector<Cards> dec;
Hands()
{
	Cards_Deck playdeck;
	playdeck.shuffle_deck();
	dec = playdeck.deck;
	for(unsigned int i = 0;i<25;i++)
	{
		allhands.push_back(dec.at(i));
	}	
}

bool straight(vector<Cards> player)
{
	if(player.at(0).getCardNumber() == 1 && player.at(1).getCardNumber() == 10 && player.at(2).getCardNumber() == 11 && player.at(3).getCardNumber() == 12 && player.at(4).getCardNumber() == 13)
	{
		return true;
	}
	else
	{
		for(unsigned int i = 0; i<player.size()-1;i++)
		{
			int c1 = player.at(i).getCardNumber()+1;
			int c2 = player.at(i+1).getCardNumber();
			if(c1 != c2)
			{
				return false;
			}
		}
		return true;
	}
}
	
int highpairs(vector<Cards> player)
{
	vector<int> pair(5,0);
	for(unsigned int i = 0;i<player.size()-1;i++)
	{
		int pairs = 1;
		int c1 = player.at(i).getCardNumber();
		for(unsigned int k = i+1;k<player.size();k++)
		{
			int c2 = player.at(k).getCardNumber();
			if(c1 == c2)
			{
				pairs++;
			}
		}
		pair.at(i) = pairs;
	}
	int high = 0;
	for(unsigned int i = 0;i<pair.size();i++)
	{
		if(pair.at(i) > high)
			high = pair.at(i);
	}
	return high;
}

float rankval(vector<Cards> player)
{
	int suitcheck = 1;
	int suit = player.at(0).getCardSuit();
	for(unsigned int k = 0; k<5;k++)
	{
		Cards ctemp = player.at(k);
		int suitT = ctemp.getCardSuit();
		if(suit != suitT)
		{
			suitcheck = 0;
		}
	}

	if(player.at(0).getCardNumber() == 1 && player.at(1).getCardNumber() == 10 && player.at(2).getCardNumber() == 11 && player.at(3).getCardNumber() == 12 && player.at(4).getCardNumber() == 13 && suitcheck == 1)
	{
		return 10;
	}
	
	//if(player.at(0).getCardNumber() == 1 and player.at(4).getCardNumber() == 13) use this code for straight
	if(straight(player) && suitcheck == 1)
	{
		float d = 9+(player.at(4).getCardNumber()/100.0);
		return d;
	}

	if(highpairs(player) == 4)//for four of a kind
	{
		float d;
		if(player.at(4).getCardNumber() == player.at(3).getCardNumber())
			d = 8+(player.at(4).getCardNumber()/100.0);
		else if((player.at(0).getCardNumber()==1&&player.at(1).getCardNumber()==1)||(player.at(3).getCardNumber()==1&&player.at(4).getCardNumber()==1))
		{
			d = 8+(14/100.0);
		}
		else
			d = 8+(player.at(1).getCardNumber()/100.0);
		return d;
	}
	
	if(highpairs(player) == 3 && suitcheck == 0)//for full house and 3 of a kind
	{
		int pairs = 1;
		for(unsigned int i = 0;i<2;i++)
		{
			int c1 = player.at(i).getCardNumber();
			int c2 = player.at(i+1).getCardNumber();
			if(c1 == c2)
			{
				pairs++;
			}
		}
		if(pairs == 3)
		{
			int c1 = player.at(3).getCardNumber();
			int c2 = player.at(4).getCardNumber();
			if(c1 == c2)
			{
				float d;
				if(player.at(1).getCardNumber() == 1)
					d = 7.14;
				else
					d = 7+(player.at(2).getCardNumber()/100.0);
				return d;
			}
		}
		pairs = 1;
		int c1 = player.at(0).getCardNumber();
		int c2 = player.at(1).getCardNumber();
		int c3 = player.at(2).getCardNumber();
		if(c1 == c2 && c2 != c3)
		{
			for(unsigned int i = 2;i<player.size()-1;i++)
			{
				int c1 = player.at(i).getCardNumber();
				int c2 = player.at(i+1).getCardNumber();
				if(c1 == c2)
				{
					pairs++;
				}
			}
			if(pairs == 3)
			{
				float d;
				if(player.at(1).getCardNumber() == 1)
					d = 7.14;
				else
					d = 7+(player.at(4).getCardNumber()/100.0);
				return d;
			}
		}
		c1 = player.at(0).getCardNumber();
		c2 = player.at(1).getCardNumber();
		if(c1 == c2)
		{
			float d;
			if(player.at(1).getCardNumber() == 1)
				d = 4.14;
			else
				d = 4+(player.at(1).getCardNumber()/100.0);
			return d;
		}
		else
		{
			float d = 4+(player.at(3).getCardNumber()/100.0);
			return d;
		}
	}
	
	if(suitcheck == 1)
	{
		float d = 6+(player.at(4).getCardNumber()/100.0);
		if(player.at(0).getCardNumber() == 1)
			d = 6.14;
		return d;
	}
	
	if(straight(player))
	{
		float d = 5+(player.at(4).getCardNumber()/100.0);
		if(player.at(4).getCardNumber() == 13 && player.at(0).getCardNumber() == 1)
			d = 5.14;
		return d;
	}
	
	if(highpairs(player) == 2)
	{	

		vector<int> pair(5,0);
		for(unsigned int i = 0;i<player.size()-1;i++)
		{
			int pairs = 1;
			int c1 = player.at(i).getCardNumber();
			for(unsigned int k = i+1;k<player.size();k++)
			{
				int c2 = player.at(k).getCardNumber();
				if(c1 == c2)
				{
					pairs++;
				}
			}
			pair.at(i) = pairs;
		}
		int numpairs = 0;
		for(unsigned int i = 0;i<pair.size();i++)
		{
			if(pair.at(i) == 2)
				numpairs++;
		}
		
		if(numpairs == 2)
		{
			for(unsigned int i = player.size()-1;i>0;i--)
			{
				int c1 = player.at(i).getCardNumber();
				int c2 = player.at(i-1).getCardNumber();
				if(c1 == c2)
				{
					float d = 3+(player.at(i).getCardNumber()/100.0);
					if(player.at(0).getCardNumber() == 1 && player.at(1).getCardNumber() == 1)
						d = 3.14;
					return d;
				}
			}
		}
	}
	
	if(highpairs(player) == 2)
	{		
		for(unsigned int i = 0;i<player.size()-1;i++)
		{
			int c1 = player.at(i).getCardNumber();
			int c2 = player.at(i+1).getCardNumber();
			if(c1 == c2)
			{
				float d = 2+(player.at(i).getCardNumber()/100.0);
				if(player.at(i).getCardNumber() == 1)
					d = 2.14;
				return d;
			}
		}
	}
	
	float d = 1+(player.at(4).getCardNumber()/100.0);
	if(player.at(0).getCardNumber() == 1)
		d = 1.14;
	return d;
}
	
int determinewinner(int numplay)
{
	vector<Cards> player1;
	for(unsigned int i = 0; i<5;i++)
	{	
		if(player1.size()<1)
		{
			player1.push_back(allhands.at(i));
		}
		else
		{	
			Cards ctemp = allhands.at(i);
			int rankN = ctemp.getCardNumber();
			int spot = -1;
			int rank;
			for(unsigned int j = 0;j<player1.size();j++)
			{
				rank = player1.at(j).getCardNumber();
				if(rankN<rank && spot == -1)
					spot = j;
			}
			if(spot == -1)
				spot = player1.size();

			player1.insert(player1.begin()+spot,allhands.at(i));
		}
	}

	vector<Cards> player2;
	for(unsigned int i = 5; i<10;i++)
	{
		if(player2.size()<1)
		{
			player2.push_back(allhands.at(i));
		}
		else
		{	
			Cards ctemp = allhands.at(i);
			int rankN = ctemp.getCardNumber();
			int spot = -1;
			int rank;
			for(unsigned int j = 0;j<player2.size();j++)
			{
				rank = player2.at(j).getCardNumber();
				if(rankN<rank && spot == -1)
					spot = j;
			}
			if(spot == -1)
				spot = player2.size();

			player2.insert(player2.begin()+spot,allhands.at(i));
		}
	}

	vector<Cards> player3;
	for(unsigned int i = 10; i<15;i++)
	{
		if(player3.size()<1)
		{
			player3.push_back(allhands.at(i));
		}
		else
		{	
			Cards ctemp = allhands.at(i);
			int rankN = ctemp.getCardNumber();
			int spot = -1;
			int rank;
			for(unsigned int j = 0;j<player3.size();j++)
			{
				rank = player3.at(j).getCardNumber();
				if(rankN<rank && spot == -1)
					spot = j;
			}
			if(spot == -1)
				spot = player3.size();

			player3.insert(player3.begin()+spot,allhands.at(i));
		}
	}

	vector<Cards> player4;
	for(unsigned int i = 15; i<20;i++)
	{
		if(player4.size()<1)
		{
			player4.push_back(allhands.at(i));
		}
		else
		{	
			Cards ctemp = allhands.at(i);
			int rankN = ctemp.getCardNumber();
			int spot = -1;
			int rank;
			for(unsigned int j = 0;j<player4.size();j++)
			{
				rank = player4.at(j).getCardNumber();
				if(rankN<rank && spot == -1)
					spot = j;
			}
			if(spot == -1)
				spot = player4.size();

			player4.insert(player4.begin()+spot,allhands.at(i));
		}
	}

	vector<Cards> player5;
	for(unsigned int i = 20; i<25;i++)
	{
		if(player5.size()<1)
		{
			player5.push_back(allhands.at(i));
		}
		else
		{	
			Cards ctemp = allhands.at(i);
			int rankN = ctemp.getCardNumber();
			int spot = -1;
			int rank;
			for(unsigned int j = 0;j<player5.size();j++)
			{
				rank = player5.at(j).getCardNumber();
				if(rankN<rank && spot == -1)
					spot = j;
			}
			if(spot == -1)
				spot = player5.size();

			player5.insert(player5.begin()+spot,allhands.at(i));
		}
	}
	
	float rank1,rank2,rank3,rank4,rank5;

	rank1 = rankval(player1);
	rank2 = rankval(player2);
	rank3 = rankval(player3);
	rank4 = rankval(player4);
	rank5 = rankval(player5);
	
	
	vector<float> winners;
  winners.push_back(rank1);
  winners.push_back(rank2);
  winners.push_back(rank3);
  winners.push_back(rank4);
  winners.push_back(rank5);


  float winval = *max_element(winners.begin(),winners.begin()+numplay);

  int winspot = 0;
  for(int w = 0;w<numplay;w++) 
  {
  if(winval == winners.at(w))
  winspot = w;
  }

  return winspot+1;
}

};
class current_player
{
public:
  std::string name;
  std::string uuid;
  int balance;
  std::vector<Cards> hand;
  current_player(std::string uuid)
  { 
    this->uuid = uuid;
  }
  void set_all(string uuid, string name, int balance)
  { 
    this->uuid = uuid;
    this->name = name;
    this->balance = balance;
  }
};
class player
{
public:
  std::string name;
  std::string uuid;
  int balance;
  Hands hand();
  player()
  {
    std::cout<<"Creating player\n";
  }
  virtual ~player()
  {
  }
  virtual void deliver(const chat_message& msg) = 0;  
};


typedef std::shared_ptr<player> player_ptr;

//----------------------------------------------------------------------
class Dealer
{
public:
  vector<Cards> hands_players;
  vector<Cards> deck;
  Hands player_hands;
  Dealer()
  {
   deck = player_hands.dec;
   hands_players = player_hands.allhands;
   in_action =NULL;
   //card_idx=0;
  }
   void new_deal()
   {
    
    Hands player_hands2;
    player_hands = player_hands2;
    deck = player_hands.dec;
    hands_players = player_hands.allhands;
   }
   /*void next_player(std::string uuid)
   {
    for(auto this_player: in_action;
   }*/
   player_ptr in_action;

};
class chat_room
{
public:
  void join(player_ptr player_)
  {
     std::cout << "joining player to the table" << std::endl;
    players_.insert(player_);
   /*for (auto msg: recent_msgs_)
    {
      participant->deliver(msg);
    }*/ 
  }

  void leave(player_ptr participant)
  {
    players_.erase(participant);
  }
  

  void deliver(const chat_message& msg) 
  {
    // CSE3310 maximum number of messages are stored in recent_msg_
    recent_msgs_.push_back(msg);  
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant: players_)
      participant->deliver(msg);
  }
  void distribute()
  {
    int num = all_players.size();
    int count =0;
    for(int i = 0;i<num;i++)
    {
      for(int j =0;j<5;j++)
      {
        all_players[i].hand.push_back(dealer.hands_players[count]);
        count++;
      }
    }
  }
  void next_player()
  {
    int total = all_players.size();
    if(current_player_index==(total-1))
    {
      current_player_index=0;
    }else{
    current_player_index++;}
  }
  
  Dealer dealer;
  int round=0;
  int swap_number = 26;
  int pot_amount;
  int min_bet_amount;
  int current_player_index;
  std::vector<current_player> all_players;
  std::set<player_ptr> players_;
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;

};

//----------------------------------------------------------------------

class chat_session
  : public player,
    public std::enable_shared_from_this<chat_session>
{
public:
  chat_session(tcp::socket socket, chat_room& room)
    : socket_(std::move(socket)),
      room_(room)
  {
    std::cout<<"Addding a player\n";
  }

  void start()
  {
      room_.join(shared_from_this()); 
      do_read_header();
  }

  void write_to_clients(std::string s)
  {
    chat_message msg;
  //converting the concatinated string to char
  char ch_arr[s.size()+1];
  std::strcpy(ch_arr,s.c_str());
  
  msg.body_length((std::strlen(ch_arr)));
  std::memcpy(msg.body(), ch_arr, msg.body_length());
  msg.encode_header();
  room_.deliver(msg);
  }
  void deliver(const chat_message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

private:
  void do_read_header()
  {
    auto self(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), chat_message::header_length),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }
//CSE3310 previous chat messages are being sent to a client in this function  
  void do_read_body()
  {
    auto self(shared_from_this());
    asio::async_read(socket_,
        asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            std::cout.write(read_msg_.body(), read_msg_.body_length());
            std::cout << "\n";
            
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//The message from the client/ player is being received in this part of //
//  the code.                                                           //
//////////////////////////////////////////////////////////////////////////
            std::string client_msg(read_msg_.body());
            client_msg = client_msg.substr(0,read_msg_.body_length());
            
            json from_client = json::parse(client_msg);
            current_player this_player(from_client["uuid"]);
            json from_dealer;
            int total_players = room_.all_players.size();
            //if it is to join the game
            if(from_client["signal"] == "join")
            {
            if(total_players<4){
              this_player.name = from_client["name"];
              this_player.balance = from_client["balance"];
              room_.all_players.push_back(this_player);
              room_.distribute();
              from_dealer["name"] = from_client["name"];
              from_dealer["uuid"] = from_client["uuid"]; 
              from_dealer["balance"] = from_client["balance"];
              from_dealer["signal"] = "joined";
              //std::cout<<from_dealer.dump();
              
              
              for(unsigned int i=0;i<room_.all_players.size();i++)
              {
                from_dealer["player"][i]["uuid"] = room_.all_players[i].uuid;
                from_dealer["player"][i]["name"] = room_.all_players[i].name;
                from_dealer["player"][i]["balance"] = room_.all_players[i].balance;
                for(int j = 0 ; j<5;j++)
                {
                  
                  from_dealer["player"][i]["card"][j]["number"] = room_.all_players[i].hand[j].getCardNumber();
                  from_dealer["player"][i]["card"][j]["suit"] = room_.all_players[i].hand[j].getCardSuit();
                }
              }
              from_dealer["num_players"] = room_.all_players.size();
              write_to_clients(from_dealer.dump());
              
            }
            else{std::cout<<"The poker table is full, please try in next round\n";}
            }
            
//////////////////////////////////////////////////////////////////////////
// when player wantes to start the game                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
            else if(from_client["signal"] == "start")
            {
            if(total_players>1){
            
              from_dealer["name"] = room_.all_players[room_.current_player_index].name;
              from_dealer["uuid"] = room_.all_players[room_.current_player_index].uuid; 
              from_dealer["signal"] = "start_game";
              from_dealer["Min bet"] = 10;
              int total_players = room_.all_players.size();
              room_.pot_amount = total_players * 10;
              from_dealer["pot"] = room_.pot_amount;
              
              //taking minimum from each players balance
              for(unsigned int i=0;i<room_.all_players.size();i++)
              {
                room_.all_players[i].balance = room_.all_players[i].balance - 10;
              }
              from_dealer["balance"] = room_.all_players[room_.current_player_index].balance;
              for(unsigned int i=0;i<room_.all_players.size();i++)
              {
                from_dealer["player"][i]["uuid"] = room_.all_players[i].uuid;
                from_dealer["player"][i]["name"] = room_.all_players[i].name;
                from_dealer["player"][i]["balance"] = room_.all_players[i].balance;
                for(int j = 0 ; j<5;j++)
                {
                  from_dealer["player"][i]["card"][j]["number"] = room_.all_players[i].hand[j].getCardNumber();
                  from_dealer["player"][i]["card"][j]["suit"] = room_.all_players[i].hand[j].getCardSuit();
                }
              }
              from_dealer["num_players"] = room_.all_players.size();
              write_to_clients(from_dealer.dump());
            }else{std::cout<<"we need more players to start the game, please wait for more players\n";}
            }
//////////////////////////////////////////////////////////////////////////
// when player presses bet                                              //
//////////////////////////////////////////////////////////////////////////
            else if(from_client["signal"] == "bet")
            {
              room_.round++;
              int numPlay = room_.all_players.size();
              if(room_.round == 3*numPlay)
              {
                int winner = room_.dealer.player_hands.determinewinner(numPlay);
                std::cout<<"The Winner is "<<room_.all_players[winner-1].name<<"! You have won $"<<room_.pot_amount<<endl;
                room_.round = 0;
                room_.all_players[winner-1].balance += room_.pot_amount;
                room_.min_bet_amount = 10;
                room_.pot_amount = 0;
                room_.dealer.new_deal();
                from_dealer["name"] = room_.all_players[room_.current_player_index].name;
                from_dealer["uuid"] = room_.all_players[room_.current_player_index].uuid; 
                from_dealer["signal"] = "start_game";
                from_dealer["Min bet"] = 10;
                int total_players = room_.all_players.size();
                room_.pot_amount = total_players * 10;
                from_dealer["pot"] = room_.pot_amount;
                room_.swap_number = 26;
              
                //taking minimum from each players balance
              for(unsigned int i=0;i<room_.all_players.size();i++)
              {
                room_.all_players[i].balance = room_.all_players[i].balance - 10;
              }
              from_dealer["balance"] = room_.all_players[room_.current_player_index].balance;
              for(unsigned int i=0;i<room_.all_players.size();i++)
              {
                from_dealer["player"][i]["uuid"] = room_.all_players[i].uuid;
                from_dealer["player"][i]["name"] = room_.all_players[i].name;
                from_dealer["player"][i]["balance"] = room_.all_players[i].balance;
                for(int j = 0 ; j<5;j++)
                {
                  from_dealer["player"][i]["card"][j]["number"] = room_.all_players[i].hand[j].getCardNumber();
                  from_dealer["player"][i]["card"][j]["suit"] = room_.all_players[i].hand[j].getCardSuit();
                }
              }
              from_dealer["num_players"] = room_.all_players.size();
              write_to_clients(from_dealer.dump());
                
              }else
              {
              /////////////////////////////////////////////////////////////
              //updating the balance of the player that has just made    //
              //the bet---Update the game state                          //
              /////////////////////////////////////////////////////////////
              for(unsigned int i=0;i<room_.all_players.size();i++)
              {
                std::string playeruuid = from_client["uuid"];
                int balance = from_client["balance"];
                if((playeruuid).compare(room_.all_players[i].uuid)==0)
                {
                  room_.all_players[i].balance = balance;
                  int betAmount = from_client["bet_amt"];
                  room_.pot_amount = room_.pot_amount + betAmount ;
                  room_.min_bet_amount = betAmount;
                }
              }
              //proceeding to next player
              room_.next_player();
             /////////////////////////////////////////////////////////////
             //sending the info to clients. how much was the bet, pot   //
             //value, minimum bet, name and balances of all players     //
             ///////////////////////////////////////////////////////////// 
             from_dealer["name"] = room_.all_players[room_.current_player_index].name;
              from_dealer["uuid"] = room_.all_players[room_.current_player_index].uuid; 
              from_dealer["signal"] = "bet_made";
              from_dealer["Min bet"] = room_.min_bet_amount;
              from_dealer["pot"] = room_.pot_amount;
              
              for(unsigned int i=0;i<room_.all_players.size();i++)
              {
                from_dealer["player"][i]["uuid"] = room_.all_players[i].uuid;
                from_dealer["player"][i]["name"] = room_.all_players[i].name;
                from_dealer["player"][i]["balance"] = room_.all_players[i].balance;
              }
              from_dealer["num_players"] = room_.all_players.size();
              write_to_clients(from_dealer.dump());
            }}
//////////////////////////////////////////////////////////////////////////
// when player presses swap                                             //
//////////////////////////////////////////////////////////////////////////
            else if(from_client["signal"]=="swap")
            {
              for(unsigned int i=0;i<room_.all_players.size();i++)
              {
                std::string playeruuid = from_client["uuid"];
                if((playeruuid).compare(room_.all_players[i].uuid)==0)
                {
                  std::string index = from_client["card_positions"];
                  /*int replacement[3];
                  replacement[0] = stoi(index.at(0));
                  replacement[1] = stoi(index.at(1));
                  replacement[2] = stoi(index.at(2));*/ 
                }
              }
            }
            else if(from_client["signal"]=="check")
            {
              //change the current player without the any bet
              room_.next_player();
              from_dealer["name"] = room_.all_players[room_.current_player_index].name;
              from_dealer["uuid"] = room_.all_players[room_.current_player_index].uuid; 
              from_dealer["signal"] = "bet_made";
              from_dealer["Min bet"] = room_.min_bet_amount;
              from_dealer["pot"] = room_.pot_amount;
              
              for(unsigned int i=0;i<room_.all_players.size();i++)
              {
                from_dealer["player"][i]["uuid"] = room_.all_players[i].uuid;
                from_dealer["player"][i]["name"] = room_.all_players[i].name;
                from_dealer["player"][i]["balance"] = room_.all_players[i].balance;
              }
              from_dealer["num_players"] = room_.all_players.size();
              write_to_clients(from_dealer.dump());

              
            }
            else if(from_client["signal"]=="allin")
            {
              //code to address all the allin condition goes here
            }
            else if(from_client["signal"]=="fold")
            {
               /*for(unsigned int i=0;i<room_.all_players.size();i++)
              {
                std::string playeruuid = from_client["uuid"];
                if((playeruuid).compare(room_.all_players[i].uuid)==0)
                {
                  room_.next_player();
                  from_dealer["name"] = room_.all_players[room_.current_player_index].name;
                  from_dealer["uuid"] = room_.all_players[room_.current_player_index].uuid; 
                  from_dealer["signal"] = "fold";
                  from_dealer["Min bet"] = room_.min_bet_amount;
                  from_dealer["pot"] = room_.pot_amount;
                  
                  room_.all_players.erase(&room_.all_players[i]);
                  
                  for(unsigned int i=0;i<room_.all_players.size();i++)
              {
                from_dealer["player"][i]["uuid"] = room_.all_players[i].uuid;
                from_dealer["player"][i]["name"] = room_.all_players[i].name;
                from_dealer["player"][i]["balance"] = room_.all_players[i].balance;
              }
              from_dealer["num_players"] = room_.all_players.size();
              write_to_clients(from_dealer.dump());
                }
              }*/
            }
            
            do_read_header();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }
// CSE3310 messages are being sent to all connected clients in this function 
  void do_write()
  {
    auto self(shared_from_this());
    asio::async_write(socket_,
        asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this, self](std::error_code ec, std::size_t /*length*/) 
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
            room_.leave(shared_from_this());
          }
        });
  }
  void write(const chat_message& msg)
  {
    asio::async_write(socket_,
        asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this, msg](std::error_code ec, std::size_t /*length*/) 			
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
            do_write();
          }
        });
  }

  tcp::socket socket_;
  chat_room& room_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
  
};

//----------------------------------------------------------------------

class chat_server
{
public:
  chat_server(asio::io_context& io_context,
      const tcp::endpoint& endpoint)
    : acceptor_(io_context, endpoint)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<chat_session>(std::move(socket), room_)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  chat_room room_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    asio::io_context io_context;

    std::list<chat_server> servers;
    for (int i = 1; i < argc; ++i)
    {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(io_context, endpoint);
    }

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
