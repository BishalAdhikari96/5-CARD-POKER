//this is a .hpp header file that containts the class deck and cards. 
//this class will be used while the games of poker starts 
#ifndef CARDS_DECK_HPP
#define CARDS_DECK_HPP

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;
// enums will hold the values of the cards and their suit in the form of numbers
  enum card_number {ACE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING};
  enum suit {DIAMONDS, HEARTS, SPADES, CLUBS};
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
  Cards_Deck()
  {  
    for(int i = DIAMONDS; i<=CLUBS; i++)
    {
      for(int j = ACE; j<=KING; j++)
      {
        Cards cardsHere(static_cast<card_number>(j),static_cast<suit>(i));
        deck.push_back(cardsHere);
      }
    }
  }
  //function to shuffle the cards
  void shuffle_deck()
  {
    random_shuffle(deck.begin(),deck.end());
  }
};
class Hands
{
	public:
		Hands();
		vector<Cards> allhands;
};
Hands::Hands()
{
	vector<Cards> dec;
	Cards_Deck playdeck;
	playdeck.shuffle_deck();
	dec = playdeck.deck;
	for(int i = 0;i<25;i++)
	{
		allhands.push_back(dec.at(i));
	}	
}
#endif
