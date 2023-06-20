#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Main.hpp>
#include "Board.h"
#include <vector>
#include <iostream>
#include <ctime>

using namespace std;
using namespace sf;

int main()
{
	srand(time(NULL));
	RenderWindow window(VideoMode(800, 600), "Minesweeper");
	Board board;

	while (window.isOpen())
	{
		Event event;
		Vector2f mousePos = (Vector2f)Mouse::getPosition(window);

		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();

			if (event.type == Event::MouseButtonPressed && Mouse::isButtonPressed(Mouse::Left))
			{
				if (board.isMouseOnTiles(mousePos) && !board.GetLoseCondition() && !board.GetWinCondition())
					board.RevealTile(mousePos);
				if (board.isMouseOnGamePiece(mousePos))
					board.GameOptions(mousePos);
			}
			if (event.type == Event::MouseButtonPressed && Mouse::isButtonPressed(Mouse::Right))
				if (board.isMouseOnTiles(mousePos) && !board.GetLoseCondition() && !board.GetWinCondition())
					board.ToggleFlag(mousePos);
		}

		window.clear(sf::Color(255, 255, 255));
		if (board.GetRestartValue())
			board.CreateBoard();

		for (int i = 0; i < 25; i++)
		{
			for (int j = 0; j < 16; j++)
			{
				window.draw(board.GetGameBoard()[i][j]->GetSprite());
				if (board.GetGameBoard()[i][j]->hasMine() && board.GetGameBoard()[i][j]->isRevealed())
					window.draw(board.GetVectorOject(board.GetGameBoard()[i][j], board.GetMineVector())->GetSprite());

				else if (board.GetGameBoard()[i][j]->hasNumber() && board.GetGameBoard()[i][j]->isRevealed())
					window.draw(board.GetVectorOject(board.GetGameBoard()[i][j], board.GetNumberVector())->GetSprite());
			}
		}
		for (int i = 0; i < board.GetObjectVector().size(); i++)
			window.draw(board.GetObjectVector().at(i)->GetSprite());

		for (int i = 0; i < board.GetMineCounterVector().size(); i++)
			window.draw(board.GetMineCounterVector().at(i)->GetSprite());

		if (board.GetDebugValue())
		{
			for (int i = 0; i < board.GetMineVector().size(); i++)
				window.draw(board.GetMineVector().at(i)->GetSprite());
		}

		for (int i = 0; i < board.GetFlagVector().size(); i++) {
			window.draw(board.GetFlagVector().at(i)->GetSprite());
		}

		window.display();
	}

	return 0;
};