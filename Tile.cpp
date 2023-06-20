#include <SFML/Graphics.hpp>
#include "GamePieces.h"
#include "Tile.h"
#include <string>

using namespace std;

Tile::Tile(string image) : GamePieces(image, name) {}

bool Tile::isFlagged()
{
	return Flag;
}

bool Tile::isRevealed()
{
	return Reveal;
}

bool Tile::hasMine()
{
	return Mine;
}

bool Tile::hasNumber()
{
	return Number;
}

void Tile::ToggleFlag()
{
	if (!Reveal)
		(Flag ? Flag = false : Flag = true);
}

void Tile::RevealTile()
{
	texture.loadFromFile("images/tile_revealed.png");
	sprite.setTexture(texture);
	Reveal = true;
}

void Tile::PlaceMine()
{
	Mine = true;
}

void Tile::RemoveMine()
{
	Mine = false;
}

void Tile::UnhideTiles()
{
	Reveal = false;
}

void Tile::SetNumber(bool hasNum)
{
	Number = hasNum;
}

vector<Tile*>& Tile::GetAdjacentTiles()
{
	return AdjacentTiles;
}