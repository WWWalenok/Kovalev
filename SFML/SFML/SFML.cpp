﻿#include <iostream>
#include <SFML/Graphics.hpp>
#include<thread>
#include<time.h>
#include <ctime>
#include <ratio>
#include <chrono>
#include <fstream>

static std::chrono::steady_clock::time_point start_time = std::chrono::high_resolution_clock::now();
const double PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;

double gausrand(double S = 1, double U = 0)
{
	return sqrt(-2 * std::log((1 + rand()) / float(RAND_MAX + 1))) * cos(2 * PI * (rand() / float(RAND_MAX))) * S + U;
}

struct PoliLine : public sf::Drawable
{
public:
	sf::Vertex* m_vertices = 0;
	uint32_t count = 0;

	PoliLine(int _count = 0, sf::Vertex* _m_vertices = 0, sf::Color _color = sf::Color(0, 0, 0, 0))
	{
		count = _count;
		if (count <= 0)
		{
			return;
		}
		if (_m_vertices == 0)
		{
			m_vertices = new sf::Vertex[count];
		}
		else
		{
			m_vertices = _m_vertices;
		}

		for (int i = 0; i < count; i++)
		{
			m_vertices[i].color = _color;
		}
	}


	sf::Vertex& operator[](uint32_t i)
	{
		return m_vertices[i % count];
	}

	void Set(sf::Vertex* n_vertices, uint32_t count)
	{
		m_vertices = n_vertices;
		PoliLine::count = count;
	}

	void SetColor(sf::Color color)
	{
		for (int i = 0; i < count; i++)
		{
			m_vertices[i].color = color;
		}
	}

private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(m_vertices, count, sf::PrimitiveType::LineStrip, states);
	}
};

struct Plot : public sf::Drawable
{
	sf::Sprite sprite;
	sf::Texture texture;
	sf::Uint8* pixels;

	int H, W;
	Plot(int H, int W)
	{
		this->H = H;
		this->W = W;

		pixels = new sf::Uint8[H * W * 4];
		texture.create(H, W);
	}

#define SetPixel(plot, x, y, r, g, b, a) {sf::Uint8* p = &plot.pixels[((x % H) * W + y % W) * 4];p[0] = r; p[1] = g; p[2] = b; p[3] = a;}

	inline void Ubdate()
	{
		texture.update(pixels);
		sprite.setTexture(texture);
	}

private:


	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(sprite);
	}
};

double
	k = 360E6,
	c = 5.7E9,
	mu = 38E9,
	a = 1,
	b = 10,
	p = 0.5E9,
	q = 2E9,
	rs = (a + b) * 0.5;

double Err(double rs)
{
	return (-p + k * (rs * rs / (b * b) - 1)) - (-q + c * k / (2 * mu + c) * (rs * rs / (a * a) - 1) + 4 * k * mu / (2 * mu + c) * std::log(rs / a));
}

void main()
{
	uint16_t H = 800, W = H;

	std::srand(std::time(0));

	int Count = 500;

	PoliLine
		Base(Count, 0, sf::Color(255, 255, 255, 128)),
		BaseExp(Count, 0, sf::Color::Yellow),
		Ideal(Count, 0, sf::Color(255, 255, 255)),
		Exp1(Count, 0, sf::Color::Red),
		Exp2(Count, 0, sf::Color(255, 0, 255)/*Pink*/),
		Ap1(Count, 0, sf::Color::Blue),
		Ap2(Count, 0, sf::Color::Green);

	float
		* ys = new float[Count],
		* xs = new float[Count];

	double ors = rs;
	{
		double
			r1 = a,
			r2 = b;
		int i = 0;
		for (; i < 10000000; i++)
		{
			double err1 = Err(r1 + (r2 - r1) * 0.25);
			err1 *= err1;
			double err2 = Err(r1 + (r2 - r1) * 0.75);
			err2 *= err2;
			if (err1 < err2)
				r2 = (r1 + r2) * 0.5;
			else
				r1 = (r1 + r2) * 0.5;

			if (err1 == 0 and err2 == 0)
				break;
		}
		std::cout << i;
		rs = (r1 + r2) * 0.5;
	}

	for (int i = 0; i < Count; i++)
		BaseExp[i].position.x =
		Ideal[i].position.x =
		Exp1[i].position.x =
		Exp2[i].position.x =
		Ap1[i].position.x =
		Ap2[i].position.x =
		Base[i].position.x;

	float BKrit = 100000000000;

	auto t_end = std::chrono::high_resolution_clock::now();
	auto t_start = std::chrono::high_resolution_clock::now();

	float
		RS = rs,
		A = k * RS * RS / (2 * mu);


	float fps = 0, K = 0;
	std::ofstream log("log.txt");


	sf::ContextSettings context_setting(0, 0, 0);
	sf::RenderWindow window(sf::VideoMode(H, W), "SFML window", sf::Style::Default, context_setting);
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);


	std::ofstream fout("out.csv");

	fout << std::scientific << "r" << ',' << "sr" << ',' << "st" << '\n';

	for (int i = 0; i < Count; i++)
	{
		float
			u = (i) / float(Count - 1),
			rs = b * u + a * (1 - u),
			x = H * 0.05 * u + H * 0.95 * (1 - u);

		float err = Err(rs);
		err = err * err;

		Base[i].position.x = x;
		Base[i].position.y = err;

		float
			sr = (rs > RS
				? -p + 2 * mu * A * (1 / (b * b) - 1 / (rs * rs))
				: -q + 2 * mu / (2 * mu + c) * (c * A * (1 / (a * a) - 1 / (rs * rs)) + 2 * k * std::log(abs(rs / a)))),

			st = (rs > RS
				? -p + 2 * mu * A * (1 / (b * b) + 1 / (rs * rs))
				: -q + 2 * mu / (2 * mu + c) * (c * A * (1 / (a * a) + 1 / (rs * rs)) + 2 * k * (1 + std::log(abs(rs / a)))));

		fout << rs << ',' << sr << ',' << st << '\n';

		Exp1[i].position.x = x;
		Exp1[i].position.y = sr * 0.000001 + W / 2.0;

		Exp2[i].position.x = x;
		Exp2[i].position.y = st * 0.000001 + W / 2.0;

		Ideal[i].position.x = x;
		Ideal[i].position.y = W / 2.0;
	}

	return;


	for (uint64_t t = 0; window.isOpen(); t++)
	{
		t_end = std::chrono::high_resolution_clock::now();
		double dt = std::chrono::duration<double, std::milli>(t_end - t_start).count() / 1000.0;
		t_start = t_end;

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		// Frame Math
		float emax = 0;

		for (int i = 0; i < Count; i++)
		{
			Base[i].position.y = Base[i].position.y / emax * W * 0.9;

		}

		window.clear();

		window.draw(Base);
		window.draw(Ideal);
		//window.draw(BaseExp);
		window.draw(Exp1);
		//window.draw(Ap1);
		window.draw(Exp2);
		//window.draw(Ap2);

		fps += 1 / dt;
		K++;
		if (t % 1 == 0)
		{
			std::cout << fps / K << "    \r";
			fps = fps * 0.05;
			K = K * 0.05;
		}
		window.display();
	}


}
