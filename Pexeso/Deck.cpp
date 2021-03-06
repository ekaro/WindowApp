#include "Deck.h"

Deck::Deck()
{		 
	for (size_t i = 0; i < Cards.size(); i++)
	{
		Cards[i] = new Card;
	}

	NewGame();	
}

Deck::~Deck()
{
	for (size_t i = 0; i < Cards.size(); i++) 
	{
		delete Cards[i];
	}
}

void Deck::ResizeDeck(const HWND& hWnd)
{
	int CurrentCardWidth = (GetClientDimensions(hWnd).first) / 5;
	int CurrentCardHeight = (GetClientDimensions(hWnd).second - offset) / 4;

	offset = GetClientDimensions(hWnd).second / 7;

	int i = 0;

	for (int m = 0; m < rows; m++)
	{
		for (int n = 0; n < columns; n++)
		{
			Cards[i]->Left = CurrentCardWidth * n;
			Cards[i]->Top = offset + CurrentCardHeight * m;
			Cards[i]->CardWidth = CurrentCardWidth;
			Cards[i]->CardHeight = CurrentCardHeight;
			
			i++;
		}
	}
}

void Deck::NewGame()
{
	for (size_t i = 0; i < Cards.size(); i++)
	{
		CardNums[i] = i % 10;
	}

	std::shuffle(std::begin(CardNums), std::end(CardNums), rng);

	for (size_t i = 0; i < Cards.size(); i++)
	{
		Cards[i]->Number = CardNums[i];
		Cards[i]->Exposed = false;
	}
	
	Turns = 0; 
	State = 0;
}

void Deck::DrawDeck(const HDC& hdc, const HWND& hWnd)
{
	ResizeDeck(hWnd);

	for (size_t i = 0; i < Cards.size(); i++)
	{
		if (Cards[i]->Exposed)
		{
			Cards[i]->Color = Blue;			
		}
		else
		{
			Cards[i]->Color = Green;
		}

		Cards[i]->DrawCard(hdc, Cards[i]->CardWidth, Cards[i]->CardHeight);

		if (Cards[i]->Exposed)
		{
			DrawNum(hdc, Cards[i]);
		}			
	}

	DrawTurns(hdc, hWnd);
}

void Deck::DrawTurns(const HDC& hdc, const HWND& hWnd)
{
	TurnsNumber = std::to_wstring(Turns);

	SetTextColor(hdc, RGB(0, 255, 0));
	SetBkColor(hdc, RGB(0, 0, 0));
	SelectObject(hdc, CardFont);

	int TurnsDistance = GetClientDimensions(hWnd).first / 5 + 10 + FontHeight / 2 * 5 + FontHeight / 10;
	
	TextOut(hdc, GetClientDimensions(hWnd).first / 5 + 10, GetClientDimensions(hWnd).second / 50, TurnsMsg, _tcslen(TurnsMsg));
	TextOut(hdc, TurnsDistance, GetClientDimensions(hWnd).second / 50, TurnsNumber.c_str(), _tcslen(TurnsNumber.c_str()));

	if (Turns < 10)
	{
		SelectObject(hdc, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hdc, RGB(0, 0, 0));
		Rectangle(hdc, TurnsDistance + (FontHeight / 2), TurnsRect.top, TurnsDistance + (FontHeight / 2) * 2, TurnsRect.top + FontHeight);
	}

	TurnsRect.left = TurnsDistance - 1;
	TurnsRect.right = TurnsDistance + (FontHeight / 2) * 2;
	TurnsRect.bottom = TurnsRect.top + FontHeight;
}

void Deck::DrawNum(const HDC& hdc, Card* Card)
{
	CardNumber = std::to_wstring(Card->Number);

	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkColor(hdc, Card->Color);
	SelectObject(hdc, CardFont);

	CardRect = Card->GetRect();
	DrawText(hdc, CardNumber.c_str(), 1, &CardRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void Deck::CompareCards(const HWND& hWnd, int Card)
{
	if (Cards[Card]->Exposed == false)
	{
		if (State == 0)
		{
			FirstCard = Card;
			Cards[FirstCard]->Exposed = true;
			State = 1;
		}
		else if (State == 1)
		{
			SecondCard = Card;
			Cards[SecondCard]->Exposed = true;
			State = 2;	
		}
		else
		{
			Turns++;
			if (CardNums[FirstCard] != CardNums[SecondCard])
			{
				Cards[FirstCard]->Exposed = false;
				Cards[SecondCard]->Exposed = false;
				CardRect = Cards[FirstCard]->GetRect();
				InvalidateRect(hWnd, &CardRect, false);
				CardRect = Cards[SecondCard]->GetRect();
				InvalidateRect(hWnd, &CardRect, false);
			}
			FirstCard = Card;
			Cards[FirstCard]->Exposed = true;
			State = 1;
		}
	}
}

void Deck::Card::DrawCard(const HDC& hdc, int CardWidth, int CardHeight) const
{
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, Color);
	Rectangle(hdc, Left, Top, Left + CardWidth, Top + CardHeight);			
}

void Deck::Clicked(const HWND& hWnd, int Card)
{
	CardRect = Cards[Card]->GetRect();

	InvalidateRect(hWnd, &CardRect, false);
	InvalidateRect(hWnd, &TurnsRect, false);
}

void Deck::ResizeText(const HWND& hWnd)
{
	int CurrentHeight = GetClientDimensions(hWnd).second;

	FontHeight = CurrentHeight / 10;
	DeleteObject(CardFont);   // delete previous font (GDI object) to prevent memory leak

	memset(&logFont, 0, sizeof(logFont));
	logFont.lfHeight = FontHeight;	
	CardFont = CreateFontIndirect(&logFont);
}

std::pair<int, int> Deck::GetClientDimensions(const HWND& hWnd)
{
	RECT ClientRect;
	::GetClientRect(hWnd, &ClientRect);

	int CurrentWidth = ClientRect.right - ClientRect.left;
	int CurrentHeight = ClientRect.bottom - ClientRect.top;

	return { CurrentWidth, CurrentHeight };
}

int Deck::GetOffset()
{
	return offset;
}

RECT Deck::Card::GetRect() const
{
	return { Left, Top, Left + CardWidth, Top + CardHeight };
}

int Deck::GetCardIndex(const HWND& hWnd, int x, int y)
{
	int CurrentCardWidth = GetClientDimensions(hWnd).first / 5;
	int CurrentCardHeight = (GetClientDimensions(hWnd).second - offset) / 4;
	int Card = (x / CurrentCardWidth) + ((y - offset) / CurrentCardHeight) * 5;

	return Card;
}