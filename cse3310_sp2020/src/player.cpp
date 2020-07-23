#include "player.hpp"
#include "chat_message.hpp"
#include "json.hpp"
#include <chrono>
#include <ctime>
#include <iostream>
#include <gtkmm.h>
#include <cstdlib>
#include <string.h>
#include <functional>
#include "asio.hpp"
#include <cstdio>

using asio::ip::tcp;
using namespace std;
using json = nlohmann::json;

class chat_client;

Person::Person()
{
  this->username = "";
  this->balance = 0;
  this->bet_amount = 0;
}

Person::Person(string name,int balance)
{
  this->username = name;
  this->balance = balance;
}

Start_menu::Start_menu(Person *p,chat_client *c):box(Gtk::ORIENTATION_VERTICAL), button_send("Send Info")
{
	this->p=p;
	this->c=c;
	set_size_request(300,200);
	set_title("Enter Info");

	add(box);
	
	label_name.set_text("Enter name: ");
	box.pack_start(label_name);

	entry_name.set_max_length(50);
	entry_name.set_text("Enter name");
	entry_name.select_region(0,entry_name.get_text_length());
	box.pack_start(entry_name);

	label_balance.set_text("Enter balance: ");
	box.pack_start(label_balance);

	entry_balance.set_max_length(50);
	entry_balance.set_text("Enter balance");
	entry_balance.select_region(0,entry_balance.get_text_length());
	box.pack_start(entry_balance);

	box.pack_start(textbox);

	button_send.signal_clicked().connect(sigc::mem_fun(*this, &Start_menu::send_info));
	box.pack_start(button_send);

	show_all_children();
}

void Start_menu::on_start_game()
{
	playerWin = new Player_window(p,c);
	c->update(playerWin);
	playerWin->show();
}

void Start_menu::send_info()
{
	p->username=entry_name.get_text();
	p->balance=stoi(entry_balance.get_text());
	on_start_game();
}

Start_menu::~Start_menu()
{}

Player_window::Player_window(Person *p,chat_client *c):box(Gtk::ORIENTATION_VERTICAL)
{
  this->c = c;  
  this->p = p;
  set_title("PLAYER WINDOW");
  add(box);
  
  pot.set_text("$--");
  pot_label.set_text("Pot Balance");
  box.pack_start(pot_label);
  box.pack_start(pot);
  bet.set_text("$--");
  turn_label.set_text("----");
  box.pack_start(turn_label);
  player1.set_text("Player 1");
  player1_balance.set_text("$---");
  player2.set_text("Player 2");
  player2_balance.set_text("$---");
  player3.set_text("Player 3");
  player3_balance.set_text("$---");
  player4.set_text("Player 3");
  player4_balance.set_text("$---");
  
  players_names.push_back(&player1);
  players_names.push_back(&player2);
  players_names.push_back(&player3);
  players_names.push_back(&player4);
  
  players_balances.push_back(&player1_balance);
  players_balances.push_back(&player2_balance);
  players_balances.push_back(&player3_balance);
  players_balances.push_back(&player4_balance);
  
  me_name.set_text(p->username);
  me_balance.set_text(std::to_string(p->balance));
  
  box2.set_size_request(800,400);
  v1.pack_start(player1,false,true,10);
  v1.pack_start(player1_balance,false,true,10);
  v1.pack_start(separatorI);
  v1.pack_start(player2,false,true,10);
  v1.pack_start(player2_balance,false,true,10);
  h2.pack_start(v1);
  table.set("./src/Card_Images/table.jpg");
  h2.pack_start(table);
  v2.pack_start(player3,false,true,10);
  v2.pack_start(player3_balance,false,true,10);
  v2.pack_start(separatorII);
  v2.pack_start(player4,false,true,10);
  v2.pack_start(player4_balance,false,true,10);
  h2.pack_start(v2);
  box2.pack_start(h2);
  box.pack_start(box2);
////////////////////////////////////////////////////////////////////////
// setting all the buttons off until the game starts and the player's 
// turn comes up
////////////////////////////////////////////////////////////////////////
  button1.set_sensitive(FALSE);
  button5.set_sensitive(FALSE);
  button10.set_sensitive(FALSE);
  button20.set_sensitive(FALSE);
  button_fold.set_sensitive(FALSE);
  button_check.set_sensitive(FALSE);
  button_swap.set_sensitive(FALSE);
  button_allin.set_sensitive(FALSE);
  button_bet.set_sensitive(FALSE);
  
//////////////////////////////////////////////////////////////////////// 

  button1.add_label("$1.00");
  
  bet_label.set_text("Your Bet amount");
  box.pack_start(bet_label);
  box.pack_start(bet);
  button1.signal_clicked().connect(sigc::mem_fun(*this,&Player_window::on_button_1));
  box3.pack_start(button1);

  button5.add_label("$5.00");
  button5.signal_clicked().connect(sigc::mem_fun(*this,&Player_window::on_button_5));
  box3.pack_start(button5);

  button10.add_label("$10.00");
  button10.signal_clicked().connect(sigc::mem_fun(*this,&Player_window::on_button_10));
  box3.pack_start(button10);

  button20.add_label("$20.00");
  button20.signal_clicked().connect(sigc::mem_fun(*this,&Player_window::on_button_20));
  box3.pack_start(button20);

  box.pack_start(box3);

  button_check.add_label("Check");
  button_check.signal_clicked().connect(sigc::mem_fun(*this,&Player_window::on_button_check));
  box4.pack_start(button_check);

  button_fold.add_label("Fold");
  button_fold.signal_clicked().connect(sigc::mem_fun(*this,&Player_window::on_button_fold));
  box4.pack_start(button_fold);

  button_allin.add_label("All In");
  button_allin.signal_clicked().connect(sigc::mem_fun(*this,&Player_window::on_button_allin));
  box4.pack_start(button_allin);

  button_swap.add_label("Swap");
  button_swap.signal_clicked().connect(sigc::mem_fun(*this,&Player_window::on_button_swap));
  box4.pack_start(button_swap);
  button_bet.add_label("Bet");
  button_bet.signal_clicked().connect(sigc::mem_fun(*this,&Player_window::on_button_bet));
  box4.pack_start(button_bet);
  box.pack_start(box4);
///////////////////////////////////////////////////////////////////
//button to request to join game and start the game
  start_game.add_label("START THE GAME \\m/");
   start_game.signal_clicked().connect(sigc::mem_fun(*this,&Player_window::on_button_start_game)); 
  start_game.set_sensitive(FALSE);
  box6.pack_start(start_game);
  
  join_game.add_label("JOIN GAME");
  join_game.signal_clicked().connect(sigc::mem_fun(*this,&Player_window::on_button_join_game)); 
  box6.pack_start(join_game);
  box.pack_start(box6);
//////////////////////////////////////// 
// setting images of the cards        //
////////////////////////////////////////  
  box5.set_size_request(50,200);
  player_hand_img1.set("./src/Card_Images/back.png");
  player_hand_img2.set("./src/Card_Images/back.png");
  player_hand_img3.set("./src/Card_Images/back.png");
  player_hand_img4.set("./src/Card_Images/back.png");
  player_hand_img5.set("./src/Card_Images/back.png");
  
  player_hand_img.push_back(&player_hand_img1);
  player_hand_img.push_back(&player_hand_img2);
  player_hand_img.push_back(&player_hand_img3);
  player_hand_img.push_back(&player_hand_img4);
  player_hand_img.push_back(&player_hand_img5);
  
  h1.pack_start(player_hand_img1,1,1);
  h1.pack_start(player_hand_img2,2,1);
  h1.pack_start(player_hand_img3,3,1);
  h1.pack_start(player_hand_img4,4,1);
  h1.pack_start(player_hand_img5,5,1);
 
 
 
  v3.pack_start(me_name);
  v3.pack_start(me_balance);
  h1.pack_start(v3);
  
  box5.pack_start(h1);
  box.pack_start(box5);
  
  show_all_children();
}

void Player_window::on_button_1()
{
  system("canberra-gtk-play -f ./src/Card_Images/chips.wav &");
  p->balance = p->balance - 1.00;
  p->bet_amount += 1;
  bet.set_text(std::to_string(p->bet_amount));
  gtk_main_iteration();
}

void Player_window::on_button_5()
{
  system("canberra-gtk-play -f ./src/Card_Images/chips.wav &");
  p->balance = p->balance - 5.00;
  p->bet_amount += 5;
  bet.set_text(std::to_string(p->bet_amount));
  gtk_main_iteration();
}

void Player_window::on_button_10()
{
  system("canberra-gtk-play -f ./src/Card_Images/chips.wav &");
  std::cout<<"bet $1\n";
  p->balance = p->balance - 10.00;
  p->bet_amount += 10;
  bet.set_text(std::to_string(p->bet_amount));
  gtk_main_iteration();
}

void Player_window::on_button_20()
{
  system("canberra-gtk-play -f ./src/Card_Images/chips.wav &");
  std::cout<<"bet $1\n";
  p->balance = p->balance - 20.00;
  p->bet_amount += 20;
  bet.set_text(std::to_string(p->bet_amount));
  gtk_main_iteration();
}

void Player_window::on_button_check()
{
  json j;
  j["uuid"] = c->get_players_ID();
  j["name"] = p->username;
  j["balance"] = p->balance;
  j["signal"] = "check";
  //creating string to save json string 
  std::string s = j.dump();
  send_to_server(s);
}

void Player_window::on_button_fold()
{
  json j;
  j["uuid"] = c->get_players_ID();
  j["name"] = p->username;
  j["balance"] = p->balance;
  j["signal"] = "fold";
  //creating string to save json string 
  std::string s = j.dump();
  send_to_server(s);
}

void Player_window::on_button_allin()
{
  json j;
  j["uuid"] = c->get_players_ID();
  j["name"] = p->username;
  j["balance"] = p->balance;
  j["signal"] = "allin";
  //creating string to save json string 
  std::string s = j.dump();
  send_to_server(s);
}

void Player_window::on_button_swap()
{
  json j;
  j["uuid"] = c->get_players_ID();
  j["name"] = p->username;
  j["balance"] = p->balance;
  j["signal"] = "swap";
  std::string s = j.dump();
  swap=new Swap_window(p,c);
  swap->show();
  //send_to_server(s);
}
void Player_window::on_button_bet()
{
  json j;
  j["uuid"] = c->get_players_ID();
  j["name"] = p->username;
  j["balance"] = p->balance-p->bet_amount;
  p->balance = p->balance-p->bet_amount;
  j["signal"] = "bet";
  j["bet_amt"] = p->bet_amount;
  //creating string to save json string 
  std::string s = j.dump();
  send_to_server(s);
}
void Player_window::on_button_join_game()
{
   json j;
   j["uuid"] = c->get_players_ID();
   j["name"] = p->username;
   j["balance"] = p->balance;
   j["signal"] = "join";
   std::string s = j.dump();
   send_to_server(s);
   start_game.set_sensitive(TRUE);
   join_game.set_sensitive(FALSE);   
}
void Player_window::on_button_start_game()
{
    json j;
    j["uuid"] = c->get_players_ID();
    j["name"] = p->username;
    j["balance"] = p->balance;
    j["signal"] = "start";
    std::string s = j.dump();
    send_to_server(s);
}
void Player_window::send_to_server(std::string s)
{
  chat_message msg;
  //converting the concatinated string to char
  char ch_arr[s.size()+1];
  std::strcpy(ch_arr,s.c_str());
  
  msg.body_length((std::strlen(ch_arr)));
  std::memcpy(msg.body(), ch_arr, msg.body_length());
  msg.encode_header();
  c->write(msg);
}

Player_window::~Player_window()
{}
Swap_window::Swap_window(Person *p,chat_client *c):box(Gtk::ORIENTATION_VERTICAL),check1("Card 1"),check2("Card 2"),check3("Card 3"),check4("Card 4"),check5("Card 5"),button_done("Done")
{
  this->p=p;
	this->c=c;
	set_size_request(300,100);
	set_title("Swap Cards!");

	add(box);

	box.pack_start(textbox);
	textbox.pack_start(check1);
	check1.set_active(false);

	box.pack_start(textbox2);
	textbox.pack_start(check2);
	check2.set_active(false);

	box.pack_start(textbox3);
	textbox.pack_start(check3);
	check3.set_active(false);

	box.pack_start(textbox4);
	textbox.pack_start(check4);
	check4.set_active(false);

	box.pack_start(textbox5);
	textbox.pack_start(check5);
	check5.set_active(false);
	
	button_done.signal_clicked().connect(sigc::mem_fun(*this,&Swap_window::done_button));
	box.pack_start(button_done);
	
	show_all_children();
}

Swap_window::~Swap_window()
{}

void Swap_window::done_button()
{
	checkbutton1();
	checkbutton2();
	checkbutton3();
	checkbutton4();
	checkbutton5();
	std::string cards="";
	for(unsigned int i = 0;i<swap_demand.size();i++)
	{
	 cards = cards+std::to_string(swap_demand[i]);
	}
	send_to_server(cards);
	hide();
}

void Swap_window::checkbutton1()
{
	if(check1.get_active()==true)
	{
		add_to_vector(0);
	}
}

void Swap_window::checkbutton2()
{
	if(check2.get_active()==true)
	{
		add_to_vector(1);
	}
}

void Swap_window::checkbutton3()
{
	if(check3.get_active()==true)
	{
		add_to_vector(2);
	}
}

void Swap_window::checkbutton4()
{
	if(check4.get_active()==true)
	{
		add_to_vector(3);
	}
}

void Swap_window::checkbutton5()
{
	if(check5.get_active()==true)
	{
		add_to_vector(4);
	}
}
void Swap_window::add_to_vector(int a)
{
  int num_of_cards = swap_demand.size();
  if(num_of_cards<4)
  {
    swap_demand.push_back(a);
  }
}
void Swap_window::send_to_server(string sig)
{
  json j;
  j["uuid"] = c->get_players_ID();
  j["name"] = p->username;
  j["balance"] = p->balance;
  j["signal"] = "swap";
  j["card_positions"] = sig;
  std::string s = j.dump();
  chat_message msg;
  //converting the concatinated string to char
  char ch_arr[s.size()+1];
  std::strcpy(ch_arr,s.c_str());

  msg.body_length((std::strlen(ch_arr)));
  std::memcpy(msg.body(), ch_arr, msg.body_length());
  msg.encode_header();
  c->write(msg);
}
