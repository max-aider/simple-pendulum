#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <iomanip>
#include <math.h>
#include <sstream>
#include <string.h>
#include <vector>



class Pendulum
{
private:

	float length = 1.0F;
	float angle = 0.0F;
	float acceleration = 0.0F;
	float velocity = 0.0F;

public:

	void setLength(float lengthInMeters)
	{
		length = lengthInMeters;
	}

	void setDegrees(float angleInDegrees)
	{
		angle = angleInDegrees / 180.0F * 3.14F;
	}

	void setRadians(float angleInRadians)
	{
		angle = angleInRadians;
	}

	float getLength() const
	{
		return length;
	}

	float getDegrees() const
	{
		return angle / 3.14F * 180.0F;
	}

	float getRadians() const
	{
		return angle;
	}

	float getAcceleration() const
	{
		return acceleration;
	}

	float getLinearAcceleration() const
	{
		return acceleration * length;
	}

	float getVelocity() const
	{
		return velocity;
	}

	float getLinearVelocity() const
	{
		return velocity * length;
	}

	void tick(float deltaTime)
	{
		acceleration = 9.81F * std::cosf(angle) / length;
		velocity += acceleration * deltaTime;
		angle += velocity * deltaTime;
	}
};



class Scene
{
private:

	float scale;
	float elapsedTime = 0.0F;
	bool playing = true;

	Pendulum& pendulum;

	sf::Clock engineClock;
	sf::Clock timerClock;

	sf::Vector2f size;
	sf::Vector2f position;

	sf::RectangleShape thread;
	sf::CircleShape ball;

	sf::Font font;
	sf::Text debugText;

public:

	Scene(const sf::RenderWindow& window, Pendulum& pendulum)
		: size(window.getSize()),
		pendulum(pendulum),
		position(size / 2.0F)
	{
		scale = size.y / 2.25F;

		thread.setSize(sf::Vector2f(scale, 16));
		thread.setFillColor(sf::Color(128, 128, 128));
		thread.setOrigin(8, 8);
		thread.setPosition(position);

		ball.setRadius(32);
		ball.setOrigin(ball.getRadius(), ball.getRadius());

		font.loadFromFile("Inconsolata.ttf");
		debugText.setFont(font);
		debugText.setCharacterSize(scale / 16);
	}

	bool isPlaying() const
	{
		return playing;
	}

	void setPlaying(bool isPlaying)
	{
		playing = isPlaying;
	}

	float getElapsedTime()
	{
		if (!playing)
		{
			timerClock.restart();
		}
		else
		{
			elapsedTime = timerClock.getElapsedTime().asSeconds();
		}

		return elapsedTime;
	}

	void tick()
	{
		if (!playing)
		{
			engineClock.restart();
			return;
		}

		pendulum.tick(engineClock.getElapsedTime().asSeconds());

		engineClock.restart();
	}

	void draw(sf::RenderWindow& window)
	{
		thread.setRotation(pendulum.getDegrees());
		ball.setPosition(position.x + scale * std::cosf(pendulum.getRadians()), position.y + scale * std::sinf(pendulum.getRadians()));

		std::stringstream ss;
		ss << std::setprecision(2) << std::fixed << std::setw(15) << std::right
			<< " Length: "
			<< std::setw(5) << std::right
			<< pendulum.getLength() << " m\n"
			<< std::setw(15) << std::right
			<< " Acceleration: "
			<< std::setw(5) << std::right
			<< pendulum.getLinearAcceleration() << " m/s^2 ("
			<< std::setw(5) << std::right
			<< pendulum.getAcceleration() << " rad/s^2)\n"
			<< std::setw(15) << std::right
			<< " Velocity: "
			<< std::setw(5) << std::right
			<< pendulum.getLinearVelocity() << " m/s   ("
			<< std::setw(5) << std::right
			<< pendulum.getVelocity() << " rad/s)\n"
			<< std::setw(15) << std::right
			<< " Elapsed: "
			<< std::setw(5) << std::right
			<< getElapsedTime() << " s";

		debugText.setString(ss.str());

		window.draw(thread);
		window.draw(ball);
		window.draw(debugText);
	}
};



int WinMain()
{
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "Sandbox", sf::Style::Close);
	window.setFramerateLimit(200);



	Pendulum pendulum;
	Scene scene(window, pendulum);



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
					scene.setPlaying(!scene.isPlaying());
				}
			}
		}

		window.clear(sf::Color::Black);

		scene.tick();
		scene.draw(window);

		window.display();
	}

	return 0;
}