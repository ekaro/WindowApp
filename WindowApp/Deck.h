#pragma once
#include <windows.h>
#include <vector>
#include <random>
#include <algorithm>

class Deck
{
private:
	class Card
	{
	public:
		Card() = default;
		void DrawCard(HDC hdc, int CardWidth, int CardHeight);
		int left = 0;
		int top;
		int CardWidth;
		int CardHeight;
		COLORREF Color;
		bool Exposed = false;
		int Number;
	private:
		HDC handle;
	};
public:
	Deck(HWND hWnd, int ButtonOffset);
	void DrawDeck(HDC hdc);    // drawing of every card in the deck
	void ResizeDeck();		   // resizing of deck according to current size of the window
	void NewGame();
	std::vector<Card> Cards;   // vector of all cards in the deck
	int CardNums[20];
private:
	HWND handle;
	int columns = 5;
	int rows = 4;
	int offset;
	std::random_device rd;
	std::default_random_engine rng = std::default_random_engine(rd()); // default_random_engine have to be seeded with a unique seed on each pass 
	                                                                   // so that numbers are different each time program is ran
	//std::mt19937 rng = std::mt19937(rd());                           // mt19937 is higher quality generator, default_random_engine is for lightweight use
};