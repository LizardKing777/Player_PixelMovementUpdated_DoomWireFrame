#ifndef EP_SPRITESET_MAP_DOOM_H
#define EP_SPRITESET_MAP_DOOM_H

#include <SDL2/SDL.h>
#include <vector>
#include "bitmap.h"

class Spriteset_MapDoom {
public:

	struct vec3 {
		float x, y, z;
	};
	struct connection {
		int a, b;
	};
	struct Point {
		float x, y;
		bool upper;
	};

	int displayX = 0;
	int displayY = 0;
	int rotationX = 0;
	int rotationY = 0;
	int rotationZ = 0;

	Spriteset_MapDoom();
	Spriteset_MapDoom(std::string n, int zoom, int dx, int dy, int rx, int ry, int rz);

	void Update();

	//Spriteset_Map* spriteset;

	BitmapRef sprite;
	BitmapRef spriteUpper;

	void pixel(float x, float y, float z);
	void line(float x1, float y1, float x2, float y2, float z1, float z2);
	void rotate(vec3& point, float x = 1, float y = 1, float z = 1);

	float lerp(float a, float b, float f);

	void Load_OBJ(std::string name);
	std::vector<std::string> split(const std::string& s, const std::string& delimiter);

	std::vector<Point> points;
	std::vector <vec3> points3D;
	std::vector <connection> connections3D;

	vec3 centeroid;

	void Show();
	SDL_Renderer* renderer;

protected:

};
#endif
