#ifndef VEC2_
#define VEC2_

#include <cmath>

struct Vec2
{
	union
	{
		struct { double x, y; };
		double n[2];
	};
	Vec2() : x(0), y(0) {}
	Vec2(double x, double y) : x(x), y(y) {}
	Vec2 operator+(Vec2 u) { return Vec2(x + u.x, y + u.y);}
	Vec2 operator-(Vec2 u) { return Vec2(x - u.x, y - u.y);}
	Vec2 operator*(double a) { return Vec2(x * a, y * a);}
	Vec2 operator/(double a) { if(a == 0) return Vec2(0, 0); return Vec2(x / a, y / a);}
	Vec2 unit() { return *this / mag();}
	double mag() { return sqrt(x * x + y * y);}
	double dot(Vec2 u) { return x * u.x + y * u.y; }
	Vec2 rotate(double angle) { return Vec2(x * cos(angle) - y * sin(angle), y * cos(angle) + x * sin(angle));}
};

#endif
