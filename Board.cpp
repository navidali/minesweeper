#include <SFML/Graphics.hpp>
#include "GamePieces.h"
#include "Board.h"
#include "Tile.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// constructors
Board::Board()
{
	float xpos = 0;
	float ypos = 0;

	GameBoard = new Tile * *[25];
	for (int i = 0; i < 25; i++)				// creating a gameboard of tiles spaced out within the window
	{
		GameBoard[i] = new Tile * [16];
		for (int j = 0; j < 16; j++)
		{
			GameBoard[i][j] = new Tile("images/tile_hidden.png");
			GameBoard[i][j]->GetSprite().setPosition(xpos, ypos);
			ypos += 32;
		}
		xpos += 32;
		ypos = 0;
	}

	SetAdjacentTiles();
	SetMines();
	CalculateNumbers();

	string images[] = { "images/face_happy.png", "", "images/debug.png", "images/test_1.png", "images/test_2.png", "images/Test_3.png" };
	string names[] = { "smiley", "", "debug", "test1", "test2", "test3" };
	GamePieces** gp = new GamePieces * [6];
	float xPos = 9.5;
	for (int i = 0; i < 7; i++)					// create game option buttons below the gameboard
	{
		if (i == 1)
		{
			xPos += 2;
			continue;
		}
		else if (i == 6)
		{
			for (int i = 0; i < 11; i++)
			{
				GamePieces* gp = new GamePieces("images/digits.png", to_string(i));
				gp->GetSprite().setTextureRect(sf::IntRect(21 * i, 0, 21, 32));
				MineCounterSprites[i] = gp;
			}
			CreateCounter();
		}
		else
		{
			gp[i] = new GamePieces(images[i], names[i]);
			gp[i]->GetSprite().setPosition((xPos += 2) * 32, 16 * 32);
			ObjectVector.push_back(gp[i]);
		}
	}
}

Board::~Board()
{
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 16; j++)
			if (GameBoard[i][j] != nullptr)
				delete[] GameBoard[i][j];
	}

	for (int i = 0; i < ObjectVector.size(); i++)
		if (ObjectVector.at(i) != nullptr)
			delete[] ObjectVector.at(i);

	for (int i = 0; i < FlagVector.size(); i++)
		if (FlagVector.at(i) != nullptr)
			delete[] FlagVector.at(i);

	for (int i = 0; i < MineVector.size(); i++)
		if (MineVector.at(i) != nullptr)
			delete[] MineVector.at(i);

	for (int i = 0; i < NumberVector.size(); i++)
		if (NumberVector.at(i) != nullptr)
			delete[] NumberVector.at(i);
}

// class functions
void Board::CreateBoard()
{												// resets gameboard to initial state
	if (loseCondition) {
		DebugVal = false;
	}
	winCondition = false;
	loseCondition = false;
	restartVal = false;

	for (int i = 0; i < 400; i++)
	{
		if (ObjFromGameBoard(i)->isFlagged())
			ObjFromGameBoard(i)->ToggleFlag();
	}
	FlagVector.clear();

	ObjFromObjectVec("smiley")->GetTexture().loadFromFile("images/face_happy.png");

	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			GameBoard[i][j]->GetTexture().loadFromFile("images/tile_hidden.png");
			GameBoard[i][j]->UnhideTiles();
		}
	}
	SetMines();
	CalculateNumbers();
	CreateCounter();

	TestMode = false;
}

void Board::CreateCounter()
{
	MineCounterVec.clear();
	int value = MineVector.size() - FlagVector.size();
	string str = to_string(value);
	int shift = 0;
	bool repeatval = false;

	for (int i = 0; i < str.length(); i++)			// each digit of the minecount is iterated through and an appropriate sprite is assigned to the counter
	{
		if (str.at(i) == '-')
		{
			MineCounterSprites[10]->GetSprite().setPosition(shift, 16 * 32);
			MineCounterVec.push_back(MineCounterSprites[10]);
		}
		else
		{
			int count = MineCounterVec.size();
			for (int j = 0; j < count; j++)
			{
				if (MineCounterVec.at(j)->GetName() != "10" && MineCounterVec.at(j)->GetName().at(0) == str.at(i))
				{
					GamePieces* gp = new GamePieces(*MineCounterVec.at(j));
					gp->GetSprite().setPosition(shift, 16 * 32);
					MineCounterVec.push_back(gp);
					repeatval = true;
				}
			}

			if (!repeatval)
			{
				MineCounterSprites[str.at(i) - '0']->GetSprite().setPosition(shift, 16 * 32);
				MineCounterVec.push_back(MineCounterSprites[str.at(i) - '0']);
			}
			repeatval = false;
		}
		shift += 18;
	}
}

void Board::RevealTile(sf::Vector2f mousePos)
{												// reveals interacted tile if conditions are met
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			if (GameBoard[i][j]->GetSprite().getGlobalBounds().contains(mousePos) && !GameBoard[i][j]->isRevealed() && !GameBoard[i][j]->isFlagged())
			{
				GameBoard[i][j]->RevealTile();
				if (!GameBoard[i][j]->hasNumber() && !GameBoard[i][j]->hasMine())
					RevealAdjacentTiles(GameBoard[i][j]);
			}

			if (GameBoard[i][j]->isRevealed() && GameBoard[i][j]->hasMine())
			{
				loseCondition = true;
				DebugVal = true;
				GameOutcome();
			}
		}
	}

	// board check if all non-mine tiles have been revealed, user wins and game is paused 
	winCondition = true;
	for (int i = 0; i < 400; i++)
	{
		if (!ObjFromGameBoard(i)->isRevealed() && !ObjFromGameBoard(i)->hasMine())
			winCondition = false;
	}
	if (winCondition)
		GameOutcome();
}

void Board::RevealAdjacentTiles(Tile* tile)
{
	vector<Tile*> tilevec;						// stores blank tiles in a vector
	for (int i = 0; i < tile->GetAdjacentTiles().size(); i++)
	{
		if (!tile->GetAdjacentTiles().at(i)->hasNumber() && !tile->GetAdjacentTiles().at(i)->isRevealed() && !tile->GetAdjacentTiles().at(i)->isFlagged() && !tile->GetAdjacentTiles().at(i)->hasMine())
			tilevec.push_back(tile->GetAdjacentTiles().at(i));
	}
	// if tile has no blank adjacent mines, reveal tiles that have numbers
	if (tilevec.size() == 0)
	{
		for (int i = 0; i < tile->GetAdjacentTiles().size(); i++)
			if (!tile->GetAdjacentTiles().at(i)->isRevealed() && !tile->GetAdjacentTiles().at(i)->isFlagged() && tile->GetAdjacentTiles().at(i)->hasNumber())
				tile->GetAdjacentTiles().at(i)->RevealTile();
	}
	// recursive loop that iterates until no more adjacent mines are blank in respect to the tile clicked
	while (tilevec.size() > 0)
	{
		if (!tilevec.at(0)->isRevealed() && !tilevec.at(0)->isFlagged() && !tilevec.at(0)->hasMine())
		{
			tilevec.at(0)->RevealTile();
			if (!tilevec.at(0)->hasNumber())
			{
				for (int i = 0; i < tilevec.at(0)->GetAdjacentTiles().size(); i++)
					if (!tilevec.at(0)->GetAdjacentTiles().at(i)->isRevealed() && !tilevec.at(0)->GetAdjacentTiles().at(i)->isFlagged() && !tilevec.at(0)->GetAdjacentTiles().at(i)->hasMine())
						tilevec.push_back(tilevec.at(0)->GetAdjacentTiles().at(i));
			}
		}
		tilevec.erase(tilevec.begin(), tilevec.begin() + 1);
	}
}

void Board::SetAdjacentTiles()
{
	for (int j = 0; j < 16; j++)				// sets adjacent mines for each tile
	{
		for (int i = 0; i < 25; i++)
		{
			if (i > 0 && j > 0)
				GameBoard[i][j]->GetAdjacentTiles().push_back(GameBoard[i - 1][j - 1]);

			if (j > 0)
				GameBoard[i][j]->GetAdjacentTiles().push_back(GameBoard[i][j - 1]);

			if (i < 24 && j > 0)
				GameBoard[i][j]->GetAdjacentTiles().push_back(GameBoard[i + 1][j - 1]);

			if (i > 0)
				GameBoard[i][j]->GetAdjacentTiles().push_back(GameBoard[i - 1][j]);

			if (i < 24)
				GameBoard[i][j]->GetAdjacentTiles().push_back(GameBoard[i + 1][j]);

			if (i > 0 && j < 15)
				GameBoard[i][j]->GetAdjacentTiles().push_back(GameBoard[i - 1][j + 1]);

			if (j < 15)
				GameBoard[i][j]->GetAdjacentTiles().push_back(GameBoard[i][j + 1]);

			if (i < 24 && j < 15)
				GameBoard[i][j]->GetAdjacentTiles().push_back(GameBoard[i + 1][j + 1]);
		}
	}
}

void Board::ToggleFlag(sf::Vector2f mousePos)
{
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 16; j++)				// create or remove a flag sprite is tile hasn't been revealed, adjustments are made to the flagvector and counter 
		{
			if (GameBoard[i][j]->GetSprite().getGlobalBounds().contains(mousePos) && !GameBoard[i][j]->isRevealed() && !GameBoard[i][j]->isFlagged())
			{
				GamePieces* gp = new GamePieces("images/flag.png", "flag");
				gp->GetSprite().setPosition(GameBoard[i][j]->GetSprite().getPosition());
				FlagVector.push_back(gp);
				CreateCounter();
				GameBoard[i][j]->ToggleFlag();
			}
			else if (GameBoard[i][j]->GetSprite().getGlobalBounds().contains(mousePos) && !GameBoard[i][j]->isRevealed() && GameBoard[i][j]->isFlagged())
			{
				for (int iterator = 0; iterator < FlagVector.size(); iterator++)
				{
					if (GameBoard[i][j]->GetSprite().getPosition() == FlagVector.at(iterator)->GetSprite().getPosition())
					{
						FlagVector.erase(FlagVector.begin() + iterator);
						GameBoard[i][j]->ToggleFlag();
					}
				}
				CreateCounter();
			}
		}
	}

	// check when counter reaches 0 to determine whether player has successfully flagged all mines 
	if (MineVector.size() - FlagVector.size() == 0 && CompareVecs(FlagVector, MineVector))
	{
		winCondition = true;
		GameOutcome();
	}
}

void Board::SetMines()
{
	if (!TestMode)									// randomly place 50 mines on the board if user hasn't selected a board test
	{
		for (int i = 0; i < 400; i++)
			if (ObjFromGameBoard(i)->hasMine())
				ObjFromGameBoard(i)->RemoveMine();

		MineVector.clear();
		int rand1, rand2;
		int counter = 0;
		while (counter < 50)
		{
			rand1 = rand() % 25;
			rand2 = rand() % 16;

			if (GameBoard[rand1][rand2]->hasMine())
				continue;
			else
			{
				GamePieces* gp = new GamePieces("images/mine.png", "mine");
				gp->GetSprite().setPosition(GameBoard[rand1][rand2]->GetSprite().getPosition());
				GameBoard[rand1][rand2]->PlaceMine();
				MineVector.push_back(gp);
				counter++;
			}
		}
	}
}

void Board::GameOptions(sf::Vector2f mousePos)
{
	if (!winCondition && !loseCondition)				// allows the user to interact with game options if the game is still playable
	{
		if (GetGamePiece(mousePos)->GetName() == "debug" && !winCondition && !loseCondition)
			ToggleDebugMode();

		else
		{
			for (int i = 1; i < 4; i++)
				if (GetGamePiece(mousePos)->GetName() == "test" + to_string(i))
				{
					LoadFile("boards/testboard" + to_string(i) + ".brd");
					TestMode = true;
					restartVal = true;
				}
		}
	}
	if (GetGamePiece(mousePos)->GetName() == "smiley")
		restartVal = true;
}

void Board::GameOutcome()
{														// replaces texture of the smiley sprite upon winning or losing
	if (loseCondition)
		ObjFromObjectVec("smiley")->GetTexture().loadFromFile("images/face_lose.png");

	else if (winCondition)
	{
		ObjFromObjectVec("smiley")->GetTexture().loadFromFile("images/face_win.png");

		FlagVector.clear();
		for (int i = 0; i < MineVector.size(); i++)
		{
			GamePieces* gp = new GamePieces("images/flag.png", "flag");
			gp->GetSprite().setPosition(MineVector.at(i)->GetSprite().getPosition());
			FlagVector.push_back(gp);
		}
		CreateCounter();
	}
}

void Board::CalculateNumbers()								// Gets the number of adjacent mines and calls a helper function to set a number for the tile
{
	for (int i = 0; i < 400; i++)
	{
		if (ObjFromGameBoard(i)->hasNumber())
			ObjFromGameBoard(i)->SetNumber(false);
	}
	NumberVector.clear();

	int AdjacentMines = 0;

	for (int j = 0; j < 16; j++)
	{
		for (int i = 0; i < 25; i++)
		{
			if (!GameBoard[i][j]->hasMine())
			{
				for (unsigned int index = 0; index < GameBoard[i][j]->GetAdjacentTiles().size(); index++)
				{
					if (GameBoard[i][j]->GetAdjacentTiles().at(index)->hasMine())
						AdjacentMines++;
				}

				SetNumbers(GameBoard[i][j], AdjacentMines);
				AdjacentMines = 0;
			}
		}
	}
}

void Board::SetNumbers(Tile* tile, int num)
{														// Stores a number corresponding to the number of mines to the NumberVector
	if (num > 0)
	{
		GamePieces* gp = new GamePieces("images/number_" + to_string(num) + ".png", to_string(num));
		gp->GetSprite().setPosition(tile->GetSprite().getPosition());
		tile->SetNumber(true);
		NumberVector.push_back(gp);
	}
}

void Board::ToggleDebugMode()
{
	if (DebugVal ? DebugVal = false : DebugVal = true);
}

bool Board::isMouseOnTiles(sf::Vector2f mousePos)
{
	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 16; j++)
			if (GameBoard[i][j]->GetSprite().getGlobalBounds().contains(mousePos))
				return true;
	}
	return false;
}

bool Board::isMouseOnGamePiece(sf::Vector2f mousePos)
{
	for (int i = 0; i < ObjectVector.size(); i++)
		if (ObjectVector.at(i)->GetSprite().getGlobalBounds().contains(mousePos))
			return true;

	return false;
}

// accessors
bool Board::GetDebugValue()
{
	return DebugVal;
}

bool Board::GetWinCondition()
{
	return winCondition;
}

bool Board::GetLoseCondition()
{
	return loseCondition;
}

bool Board::GetRestartValue()
{
	return restartVal;
}

Tile*** Board::GetGameBoard()
{
	return GameBoard;
}

vector<GamePieces*>& Board::GetFlagVector()
{
	return FlagVector;
}

vector<GamePieces*>& Board::GetObjectVector()
{
	return ObjectVector;
}

vector<GamePieces*>& Board::GetMineVector()
{
	return MineVector;
}

vector<GamePieces*>& Board::GetNumberVector()
{
	return NumberVector;
}

vector<GamePieces*>& Board::GetMineCounterVector()
{
	return MineCounterVec;
}

// helper functions
void Board::LoadFile(string filename)
{
	for (int i = 0; i < 400; i++)
		if (ObjFromGameBoard(i)->hasMine())
			ObjFromGameBoard(i)->RemoveMine();

	MineVector.clear();
	fstream file(filename, ios_base::in);
	int index = 0;
	string line;

	if (file.is_open())
	{
		while (!file.eof())
		{
			getline(file, line);
			for (int i = 0; i < 25; i++)
			{
				if (line.at(i) == '1')				// has mine
				{
					GamePieces* gp = new GamePieces("images/mine.png", "mine");
					gp->GetSprite().setPosition(ObjFromGameBoard(index)->GetSprite().getPosition());
					ObjFromGameBoard(index)->PlaceMine();
					MineVector.push_back(gp);
				}
				index++;
			}
		}
	}
}

bool Board::CompareVecs(vector<GamePieces*>& vec1, vector<GamePieces*>& vec2)
{
	bool hasElement = false;
	int count = 0;
	for (unsigned int i = 0; i < vec1.size(); i++)
	{
		for (unsigned int j = 0; j < vec2.size() && !hasElement; j++)
		{
			if (j == vec2.size() - 1 && vec1.at(i)->GetSprite().getPosition() != vec2.at(j)->GetSprite().getPosition() && !hasElement)
				return false;

			else
			{
				if (vec1.at(i)->GetSprite().getPosition() == vec2.at(j)->GetSprite().getPosition())
				{
					hasElement = true;
					count++;
				}
			}
		}
		hasElement = false;
	}

	if (count = vec1.size())
		return true;

	else
		return false;
}

GamePieces* Board::GetGamePiece(sf::Vector2f mousePos)
{
	for (int i = 0; i < ObjectVector.size(); i++)
		if (ObjectVector.at(i)->GetSprite().getGlobalBounds().contains(mousePos))
			return ObjectVector.at(i);
}

GamePieces* Board::GetVectorOject(Tile* tile, vector<GamePieces*>& vec)
{
	for (int i = 0; i < vec.size(); i++)
		if (vec.at(i)->GetSprite().getPosition() == tile->GetSprite().getPosition())
			return vec.at(i);
}

Tile* Board::ObjFromGameBoard(int index)
{
	int iterator = 0;
	for (int j = 0; j < 16; j++)
	{
		for (int i = 0; i < 25; i++)
		{
			if (iterator == index)
				return GameBoard[i][j];

			iterator++;
		}
	}
}

GamePieces* Board::ObjFromObjectVec(string name)
{
	for (int i = 0; i < ObjectVector.size(); i++)
	{
		if (ObjectVector.at(i)->GetName() == name)
			return ObjectVector.at(i);
	}
}