#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>

bool collision(float x, float y, float w, float h, float x1, float y1, float w1, float h1) {
	if (x + w >= x1 && x <= x1 + w1 && y + h >= y1 && y <= y1 + h1) {
		return true;
	}
	return false;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(1000, 600), "Flappy Bird");
	
	window.setFramerateLimit(70);
	window.setKeyRepeatEnabled(false);
	
	srand(time(0));

	std::vector<sf::Sprite> pipes;

	enum GameState {
		waiting,
		started,
		gameover
	};

	struct Sounds {
		sf::SoundBuffer scoringBuffer;
		sf::SoundBuffer flappingBuffer;
		sf::SoundBuffer crashingBuffer;
		sf::Sound score;
		sf::Sound flap;
		sf::Sound crash;
	} sounds;

	struct Textures {
		sf::Texture flappyBird[3];
		sf::Texture pipe;
		sf::Texture background;
		sf::Texture gameoverImg;
	} textures;

	struct FlappyBird {
		double verticalSpeed = 0;
		int frame = 0;
		sf::Sprite sprite;
	} flappyBird;

	struct Game {
		int score = 0;
		int highscore = 0;
		int frames = 0;

		GameState gameState = waiting;

		sf::Sprite background[3];
		sf::Sprite gameover;

		sf::Text pressC;
		sf::Text scoreText;
		sf::Text highscoreText;

		sf::Font font;
	} game;


	// Sounds
	sounds.scoringBuffer.loadFromFile("./audio/score.wav");
	sounds.flappingBuffer.loadFromFile("./audio/flap.wav");
	sounds.crashingBuffer.loadFromFile("./audio/crash.wav");

	sounds.score.setBuffer(sounds.scoringBuffer);
	sounds.flap.setBuffer(sounds.flappingBuffer);
	sounds.crash.setBuffer(sounds.crashingBuffer);

	// Textures
	textures.background.loadFromFile("./images/background.png");
	textures.gameoverImg.loadFromFile("./images/gameover.png");

	textures.pipe.loadFromFile("./images/pipe.png");
	textures.flappyBird[0].loadFromFile("./images/flappy1.png");
	textures.flappyBird[1].loadFromFile("./images/flappy2.png");
	textures.flappyBird[2].loadFromFile("./images/flappy3.png");

	// Flappy Bird
	flappyBird.sprite.setPosition(250, 300);
	flappyBird.sprite.setScale(2, 2);

	// Game
	game.font.loadFromFile("./fonts/flappy.ttf");

	game.background[0].setTexture(textures.background);
	game.background[1].setTexture(textures.background);
	game.background[2].setTexture(textures.background);
	game.background[0].setScale(1.15625, 1.171875);
	game.background[1].setScale(1.15625, 1.171875);
	game.background[2].setScale(1.15625, 1.171875);
	game.background[1].setPosition(333, 0);
	game.background[2].setPosition(666, 0);

	game.gameover.setTexture(textures.gameoverImg);
	game.gameover.setOrigin(192 / 2, 42 / 2);
	game.gameover.setPosition(500, 125);
	game.gameover.setScale(2, 2);

	game.pressC.setString("Press C to continue");
	game.pressC.setFont(game.font);
	game.pressC.setFillColor(sf::Color::White);
	game.pressC.setCharacterSize(50);
	game.pressC.setOrigin(game.pressC.getLocalBounds().width / 2, 0);
	game.pressC.setPosition(500, 250);

	game.scoreText.setFont(game.font);
	game.scoreText.setFillColor(sf::Color::White);
	game.scoreText.setCharacterSize(75);
	game.scoreText.move(30, 0);

	game.highscoreText.setFont(game.font);
	game.highscoreText.setFillColor(sf::Color::White);
	game.highscoreText.move(30, 80);

	while (window.isOpen()) {
		flappyBird.sprite.setTexture(textures.flappyBird[1]);

		game.scoreText.setString(std::to_string(game.score));
		game.highscoreText.setString("HIGHEST " + std::to_string(game.highscore));

		float fx = flappyBird.sprite.getPosition().x;
		float fy = flappyBird.sprite.getPosition().y;
		float fw = 34 * flappyBird.sprite.getScale().x;
		float fh = 24 * flappyBird.sprite.getScale().y;

		if (game.gameState == waiting || game.gameState == started) {
			if (game.frames % 6 == 0) {
				flappyBird.frame += 1;
			}
			
			if (flappyBird.frame == 3) {
				flappyBird.frame = 0;
			}

		}

		flappyBird.sprite.setTexture(textures.flappyBird[flappyBird.frame]);

		// Moving the Flappy
		if (game.gameState == started) {
			flappyBird.sprite.move(0, flappyBird.verticalSpeed);
			flappyBird.verticalSpeed += 0.5;
		}

		// Checking if the Flappy is touching the ground or ceiling
		if (game.gameState == started) {
			if (fy < 0) {
				flappyBird.sprite.setPosition(250, 0);
				flappyBird.verticalSpeed = 0;
			}

			else if (fy > 600) {
				flappyBird.verticalSpeed = 0;
				game.gameState = gameover;
				sounds.crash.play();
			}

		}

		// Counting the score
		for (auto& pipe : pipes) {
			if (game.gameState == started && pipe.getPosition().x == 250) {
				game.score++;
				sounds.score.play();

				if (game.score > game.highscore) {
					game.highscore = game.score;
				}

				break;
			}
		}

		// Pipe generation
		if (game.gameState == started && game.frames % 150 == 0) {
			int r = rand() % 275 + 75;
			int gap = 150;

			// Lower pipe
			sf::Sprite pipeL;
			pipeL.setTexture(textures.pipe);
			pipeL.setPosition(1000, r + gap);
			pipeL.setScale(2, 2);

			// Upper pipe
			sf::Sprite pipeU;
			pipeU.setTexture(textures.pipe);
			pipeU.setPosition(1000, r);
			pipeU.setScale(2, -2);

			// Push it to the array
			pipes.push_back(pipeL);
			pipes.push_back(pipeU);
		}

		// Move the pipes
		if (game.gameState == started) {
			for (auto& pipe : pipes) {
				pipe.move(-3, 0);
			}
		}

		// Remove the pipes if they are offscreen
		if (game.frames % 100 == 0) {
			auto startitr = pipes.begin();
			auto enditr = pipes.begin();

			for (; enditr != pipes.end(); enditr++) {
				if (enditr->getPosition().x > -104) {
					break;
				}
			}

			pipes.erase(startitr, enditr);
		}

		// Detecting collisions
		if (game.gameState == started) {
			for (auto& pipe : pipes) {

				float px, py, pw, ph;

				if (pipe.getScale().y > 0) {
					px = pipe.getPosition().x;
					py = pipe.getPosition().y;
					pw = 52 * pipe.getScale().x;
					ph = 320 * pipe.getScale().y;
				}
				else {
					pw = 52 * pipe.getScale().x;
					ph = -320 * pipe.getScale().y;
					px = pipe.getPosition().x;
					py = pipe.getPosition().y - ph;
				}

				if (collision(fx, fy, fw, fh, px, py, pw, ph)) {
					game.gameState = gameover;
					sounds.crash.play();
				}
			}
		}

		// Events
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Space) {
				if (game.gameState == waiting) {
					game.gameState = started;
				}

				if (game.gameState == started) {
					flappyBird.verticalSpeed = -8;
					sounds.flap.play();
				}

			}
			else if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::C &&
				game.gameState == gameover) {
				game.gameState = waiting;
				flappyBird.sprite.setPosition(250, 300);
				game.score = 0;
				pipes.clear();
			}
		}

		window.clear();
		window.draw(game.background[0]);
		window.draw(game.background[1]);
		window.draw(game.background[2]);
		window.draw(flappyBird.sprite);


		for (auto& pipe : pipes) {
			window.draw(pipe);
		}

		window.draw(game.scoreText);
		window.draw(game.highscoreText);

		if (game.gameState == gameover) {
			window.draw(game.gameover);

			if (game.frames % 60 < 30) {
				window.draw(game.pressC);
			}
		}
		window.display();

		game.frames++;
	}

	return 0;
}
