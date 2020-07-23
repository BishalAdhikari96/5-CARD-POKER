#ifndef PLAYER_H
#define PLAYER_H
#include <gtkmm.h>
#include "chat_client.hpp"
#include <vector>
#include "asio.hpp"

using asio::ip::tcp;

class chat_client;
class Person{
public:
  Person();
  Person(std::string name, int balance);
  std::string username;
  int balance;
  int bet_amount;
};
class Swap_window:public Gtk::Window
{
	chat_client *c;
	Person *p;
	std::vector<int> swap_demand;
	public:
	Swap_window(Person *p,chat_client *c);
	virtual ~Swap_window();
	
	protected:
	void checkbutton1();
	void checkbutton2();
	void checkbutton3();
	void checkbutton4();
	void checkbutton5();
	void done_button();
	void add_to_vector(int a);
	void send_to_server(std::string sig);

	Gtk::Box box,textbox,textbox2,textbox3,textbox4,textbox5;
	Gtk::CheckButton check1,check2,check3,check4,check5;
	Gtk::Button button_done;
};

class Player_window : public Gtk::Window
{
public:
  Person *p;
  chat_client *c;
  Swap_window *swap;
  Player_window(Person *p,chat_client *c);
  virtual ~Player_window();

  void on_button_1();
  void on_button_5();
  void on_button_10();
  void on_button_20();
  void on_button_fold();
  void on_button_check();
  void on_button_swap();
  void on_button_allin();
  void on_button_bet();
  void on_button_join_game();
  void on_button_start_game();
  void send_to_server(std::string sig);
  void receiver_function(std::string s);

  Gtk::Box box, box2,box3, box4,box5,box6;
  Gtk::HBox h1,h2;//,h3,h4,h5;
  Gtk::VBox v1,v2,v3,pot_line,bet_line;
  Gtk::Layout layout;
  Gtk::Button button1,button5,button10,button20,button_fold,button_check,button_swap,button_allin,button_bet;
  Gtk::Button start_game,join_game;
  std::vector<Gtk::Image*> player_hand_img;
  Gtk::Image player_hand_img1;
  Gtk::Image player_hand_img2;
  Gtk::Image player_hand_img3;
  Gtk::Image player_hand_img4;
  Gtk::Image player_hand_img5;
  Gtk::Image table;
  Gtk::Label player1,player2,player3,player4,player1_balance,player2_balance,player3_balance,player4_balance,me_name,me_balance;
  Gtk::Label bet;
  Gtk::Label bet_label;
  Gtk::Label pot;
  Gtk::Label pot_label;
  Gtk::Label turn_label;
  std::vector<Gtk::Label*> players_names;
  std::vector<Gtk::Label*> players_balances;
  Gtk::Separator separatorI,separatorII;
  Gtk::Grid grid;
  
};
class Start_menu : public Gtk::Window
{
	Person *p;
	chat_client *c;
public:
	Start_menu(Person *p,chat_client *c);
	virtual ~Start_menu();

protected:
	void send_info();
	void on_start_game();

	Gtk::Box textbox, box;
	Gtk::Label label_name, label_balance;
	Gtk::Entry entry_name, entry_balance;
	Gtk::Button button_close, button_send;

	Player_window *playerWin;
};
#endif
