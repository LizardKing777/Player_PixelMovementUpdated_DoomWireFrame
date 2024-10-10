#ifndef EP_SPRITESET_MAP_DOOM_H
#define EP_SPRITESET_MAP_DOOM_H

#include <SDL2/SDL.h>
#include <vector>
#include "bitmap.h"

class Spriteset_MapDoom {
public:


	struct vec3 {
		float x, y, z = 0;
	};
	struct connection {
		int a, b;
	};
	struct Point {
		float x, y, z;
		bool upper;
		Color color;

		bool operator > (const Point& str) const
		{
			return (z > str.z);
		}
	};


	int displayX = 0;
	int displayY = 0;
	int rotationX = 0;
	int rotationY = 0;
	int rotationZ = 0;

	int timer = 0;

	Spriteset_MapDoom();
	Spriteset_MapDoom(std::string n, int zoom, int dx, int dy, int rx, int ry, int rz);

	void Update();

	BitmapRef sprite;
	BitmapRef spriteUpper;

	void pixel(float x, float y, float z, Color c);
	void line(Point p1, Point p2);
	void rotate(Point& point, float x = 1, float y = 1, float z = 1);

	Spriteset_MapDoom::Point computeCentroid(const std::vector<Spriteset_MapDoom::Point>& points);
	void sortPoints(std::vector<Spriteset_MapDoom::Point>& points);
	void drawPolygon(std::vector<Point> vertices);
	bool comparePoints(const Spriteset_MapDoom::Point& p1, const Spriteset_MapDoom::Point& p2);

	float lerp(float a, float b, float f);

	void Load_OBJ(std::string name);
	std::vector<std::string> split(const std::string& s, const std::string& delimiter);

	std::vector<Point> points;
	std::vector <Point> points3D;
	std::vector <connection> connections3D;


	std::vector <std::vector<Point>> surfaces;

	vec3 centeroid;

	void Show();
	SDL_Renderer* renderer;

protected:

};
#endif
