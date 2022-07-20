#include <SFML/Graphics.hpp>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>



class Pendulum
{
private:

	float length = 1.0f;
	float angle = 0.0f;
	float acceleration = 0.0f;
	float velocity = 0.0f;

public:

	void setLength(float lengthInMeters)
	{
		length = lengthInMeters;
	}

	void setDegrees(float angleInDegrees)
	{
		angle = angleInDegrees / 180.0f * 3.14f;
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
		return angle / 3.14f * 180.0f;
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
		acceleration = 9.81f * std::cosf(angle) / length;
		velocity += acceleration * deltaTime;
		angle += velocity * deltaTime;
	}
};



class Scene
{
private:

	float scale;
	float elapsedTime = 0.0f;
	bool playing = false;

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
		position(size / 2.0f)
	{
		scale = size.y / 2.25f;

		thread.setSize(sf::Vector2f(scale, 16.0f));
		thread.setFillColor(sf::Color(127u, 127u, 127u));
		thread.setOrigin(8.0f, 8.0f);
		thread.setPosition(position);

		ball.setRadius(32.0f);
		ball.setOrigin(ball.getRadius(), ball.getRadius());

		font.loadFromFile("Inconsolata.ttf");
		debugText.setFont(font);
		debugText.setCharacterSize(scale / 16.0f);
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
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "Pendulum", sf::Style::None);
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
			else if (event.type == sf::Event::LostFocus)
			{
				scene.setPlaying(false);
			}
		}

		window.clear(sf::Color::Black);

		scene.tick();
		scene.draw(window);

		window.display();
	}

	return 0;
}