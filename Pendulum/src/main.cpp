#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <vector>
#include <math.h>
#include <string>



namespace Scene
{
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	float scale = 0;
	bool isPaused = false;

	sf::Vector2f position(desktop.width / 2, desktop.height / 10);

	sf::Clock clock;
	float lastTime = 0;

	float getTime()
	{
		if (isPaused)
		{
			clock.restart();
		}
		else
		{
			lastTime = clock.getElapsedTime().asSeconds();
		}

		return lastTime;
	}
};



class Pendulum
{
public:
	Pendulum(float length, float angle) : length(length), angle(angle / 180 * 3.14), acceleration(0), velocity(0)
	{
		Scene::scale = 1000 / length;

		thread.setSize(sf::Vector2f(length * Scene::scale, 16));
		thread.setFillColor(sf::Color(128, 128, 128));
		thread.setOrigin(8, 8);
		thread.setPosition(Scene::position);

		ball.setRadius(64);
		ball.setOrigin(ball.getRadius(), ball.getRadius());
	}

	float getLength()
	{
		return length;
	}

	float getAcceleration()
	{
		return acceleration;
	}

	float getVelocity()
	{
		return velocity;
	}

	void tick()
	{
		if (Scene::isPaused)
		{
			clock.restart();
			return;
		}

		acceleration = 9.81 * cosf(angle) / length;

		velocity += acceleration * clock.getElapsedTime().asSeconds();

		angle += velocity * clock.getElapsedTime().asSeconds();

		thread.setRotation(angle * 180 / 3.14);
		ball.setPosition(Scene::position.x + length * cosf(angle) * Scene::scale, Scene::position.y + length * sinf(angle) * Scene::scale);

		clock.restart();
	}

	void draw(sf::RenderWindow& window)
	{
		window.draw(thread);
		window.draw(ball);
	}

private:
	float length;
	float angle;
	float acceleration;
	float velocity;

	sf::RectangleShape thread;
	sf::CircleShape ball;
	sf::Clock clock;
};



int WinMain()
{
	Scene::desktop = sf::VideoMode::getDesktopMode();

	sf::RenderWindow window(Scene::desktop, "Sandbox", sf::Style::Fullscreen);
	window.setFramerateLimit(200);



	Pendulum pendulum(1, 30);



	sf::Font font;
	font.loadFromFile("Inconsolata.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(48);



	sf::Event event;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}
				else if (event.key.code == sf::Keyboard::Space)
				{
					Scene::isPaused = !Scene::isPaused;
				}
			}
		}



		window.clear(sf::Color::Black);

		pendulum.tick();
		pendulum.draw(window);

		window.draw(text);

		text.setString(" Length: " + std::to_string(pendulum.getLength())
			+ "\n Acceleration: " + std::to_string(pendulum.getAcceleration() * pendulum.getLength()) + " m/s^2 (" + std::to_string(pendulum.getAcceleration()) + " rad/s^2)"
			+ "\n Velocity: " + std::to_string(pendulum.getVelocity() * pendulum.getLength()) + " m/s (" + std::to_string(pendulum.getVelocity()) + " rad/s)"
			+ "\n Elapsed: " + std::to_string(Scene::getTime()) + " s");

		window.display();
	}

	return 0;
}