#ifndef EP_SPRITESET_MAP_DOOM_H
#define EP_SPRITESET_MAP_DOOM_H

#include <SDL2/SDL.h>
#include <vector>
#include "bitmap.h"
#include <tilemap.h>
#include "async_handler.h"
#include "sprite.h"
#include "scene.h"

class Spriteset_MapDoom {
public:

	bool doomMap = false;
	int map[999][999];

	int mapW, mapH = 0;

	// Structure repr�sentant le joueur
	struct PlayerDoom {
		float x, y;   // Position du joueur
		float angle;  // Angle de vue (orientation du joueur)
		float fov;    // Champ de vision du joueur (Field of View)
		float FOVangle = 110;
	};
	PlayerDoom player = { 10 / 2.0 * TILE_SIZE, 8 / 2.0 * TILE_SIZE, 0, 110.0f * (M_PI / 180.0f) };

	// 
	struct DrawingDoom {
		int type; // 0 => Wall, 1 => Event
		int x;
		float distance; 
		int textureX; 
		int evID;
		Point position;

		bool operator > (const DrawingDoom& d) const
		{
			return (distance > d.distance);
		}
	};

	struct vec3 {
		float x, y, z = 0;
	};
	struct connection {
		int a, b;
	};
	struct Point {
		float x, y, z = -99999999;
		bool upper = false;
		Color color;
		bool exist = false;

		bool operator > (const Point& str) const
		{
			return (z < str.z);
		}
	};

	struct EventWall {
		int x, y, id, type;
	};
	std::vector<EventWall> events_wall;

	float castRay7();
	float castRay(float rayAngle, int &ray, std::vector<DrawingDoom> &d, int x, int &mx, int &my);
	void renderScene();
	void renderMode7();

	void renderTexturedFloor(float x, float distance, float angle);
	void renderTexturedFloor();
	BitmapRef mapTexture(int x, int y);
	void OnTitleSpriteReady(FileRequestResult* result, int i);
	BitmapRef bitmap;
	BitmapRef bitmap2;

	BitmapRef chipset;
	FileRequestBinding request_id;

	int mapWidth();
	int mapHeight();

	int displayX = 0;
	int displayY = 0;
	int rotationX = 0;
	int rotationY = 0;
	int rotationZ = 0;

	double angleX, angleY, angleZ = 0;

	int timer = 0;

	Spriteset_MapDoom();
	Spriteset_MapDoom(std::string n, int zoom, int dx, int dy, int rx, int ry, int rz);

	void setRotation(int rx, int ry, int rz);
	void getRotation(int varX, int varY, int varZ);
	void Spriteset_MapDoom::normalizeAngle(double& angle);

	void Update(bool first);

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


	int refresh_index = 0;
	int refresh[6] = { 1,2,3,4,6,10 };


	Scene* scene_map = nullptr;
	//std::shared_ptr<Scene_Map*> scene_map;
	//std::unique_ptr<Scene_Map*> scene_map;
	//std::unique_ptr<Scene_Map> scene_map;
	//BitmapRef mapTextures[9999][999];
	BitmapRef mapTexturesID[9999];

	float scaleX = 96;
	float scaleY = scaleX;

protected:

};
#endif
