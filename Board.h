#pragma once
#include <SFML/Graphics.hpp>
#include "GamePieces.h"
#include "Tile.h"
#include <vector>

using namespace std;

class Board
{
private:
	Tile*** GameBoard;
	vector<GamePieces*> ObjectVector;
	vector<GamePieces*> FlagVector;
	vector<GamePieces*> MineVector;
	vector<GamePieces*> NumberVector;
	vector<GamePieces*> MineCounterVec;
	GamePieces* MineCounterSprites[11];
	bool DebugVal = false;
	bool winCondition = false;
	bool loseCondition = false;
	bool restartVal = false;
	bool TestMode = false;

public:
	// constructors
	Board();
	~Board();

	// class functions
	void CreateBoard();
	void CreateCounter();
	void RevealTile(sf::Vector2f mousePos);
	void RevealAdjacentTiles(Tile* tile);
	void SetAdjacentTiles();
	void ToggleFlag(sf::Vector2f mousePos);
	void SetMines();
	void GameOptions(sf::Vector2f mousePos);
	void GameOutcome();
	void CalculateNumbers();
	void SetNumbers(Tile* tile, int num);
	void ToggleDebugMode();
	bool isMouseOnTiles(sf::Vector2f mousePos);
	bool isMouseOnGamePiece(sf::Vector2f mousePos);

	// acessors
	bool GetDebugValue();
	bool GetWinCondition();
	bool GetLoseCondition();
	bool GetRestartValue();
	Tile*** GetGameBoard();
	vector<GamePieces*>& GetFlagVector();
	vector<GamePieces*>& GetObjectVector();
	vector<GamePieces*>& GetMineVector();
	vector<GamePieces*>& GetNumberVector();
	vector<GamePieces*>& GetMineCounterVector();

	// helper functions
	void LoadFile(string filename);
	bool CompareVecs(vector<GamePieces*>& vec1, vector<GamePieces*>& vec2);
	GamePieces* GetVectorOject(Tile* tile, vector<GamePieces*>& vec);
	GamePieces* GetGamePiece(sf::Vector2f mousePos);
	GamePieces* ObjFromObjectVec(string name);
	Tile* ObjFromGameBoard(int index);
};