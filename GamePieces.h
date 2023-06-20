#pragma once
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

class GamePieces		// parent class 
{
protected:
	sf::Sprite sprite;
	sf::Texture texture;
	string name;

public:
	GamePieces(string image, string name)
	{
		texture.loadFromFile(image);
		sprite.setTexture(texture);
		this->name = name;
	}
	virtual sf::Sprite& GetSprite() { return sprite; }
	virtual sf::Texture& GetTexture() { return texture; }
	virtual string GetName() { return name; }
};