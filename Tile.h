#pragma once
#include <SFML/Graphics.hpp>
#include "GamePieces.h"
#include <string>
#include <vector>

class Tile : public GamePieces		// child class
{
private:
	bool Flag = false;
	bool Reveal = false;
	bool Mine = false;
	bool Number = false;
	vector<Tile*> AdjacentTiles;

public:
	Tile(string name);
	bool isFlagged();
	bool isRevealed();
	bool hasMine();
	bool hasNumber();
	void ToggleFlag();
	void RevealTile();
	void PlaceMine();
	void RemoveMine();
	void UnhideTiles();
	void SetNumber(bool hasNum);
	vector<Tile*>& GetAdjacentTiles();
};