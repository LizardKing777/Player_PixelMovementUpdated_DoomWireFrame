#include <spritesetmap_doom.h>
#include <output.h>
#include "bitmap.h"
#include <filefinder.h>
#include <player.h>


Spriteset_MapDoom::Spriteset_MapDoom() {
	//renderer = SDL_GetRenderer(SDL_GetGrabbedWindow());
	//for (int i = 0;i<100; i++) {
	//	pixel(i,i);
	//}

	//line(0, 0, 64, 0);

	// Cube
	points3D = { {100,100,-50}, {200,100,-50}, {200,200,-50}, {100,200,-50},
		{100,100,50}, {200,100,50}, {200,200,50}, {100,200,50} };

	connections3D = { {0,4}, {1,5}, {2,6}, {3,7},
		{0,1}, {1,2}, {2,3}, {3,0},
		{4,5}, {5,6},{6,7}, {7,4} };
	//


	centeroid = { 0,0,0 };
	for (auto& p : points3D) {
		centeroid.x += p.x;
		centeroid.y += p.y;
		centeroid.z += p.z;
	}
	centeroid.x /= points3D.size();
	centeroid.y /= points3D.size();
	centeroid.z /= points3D.size();

	for (auto& p : points3D) {
		p.x -= centeroid.x;
		p.y -= centeroid.y;
		p.z -= centeroid.z;
		//rotate(p, 0.08, 0, 0);
		p.x += centeroid.x;
		p.y += centeroid.y;
		p.z += centeroid.z;
	}

	sprite = Bitmap::Create(480, 256);
	spriteUpper = Bitmap::Create(480, 256);
}

Spriteset_MapDoom::Spriteset_MapDoom(std::string n, int zoom, int dx, int dy, int rx, int ry, int rz) {

	Load_OBJ(n);

	displayX = dx;
	displayY = dy;
	rotationX = rx;
	rotationY = ry;
	rotationZ = rz;

	for (auto& p : points3D) {
		p.x *= zoom;
		p.y *= zoom;
		p.z *= zoom;
	}

	centeroid = { 0,0,0 };
	for (auto& p : points3D) {
		centeroid.x += p.x;
		centeroid.y += p.y;
		centeroid.z += p.z;
	}
	centeroid.x /= points3D.size();
	centeroid.y /= points3D.size();
	centeroid.z /= points3D.size();

	sprite = Bitmap::Create(Player::screen_width, Player::screen_height);
	spriteUpper = Bitmap::Create(Player::screen_width, Player::screen_height);
}


std::vector<std::string> Spriteset_MapDoom::split(const std::string& s, const std::string& delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

void Spriteset_MapDoom::Load_OBJ(std::string name) {
	points3D = {};
	connections3D = {};

	std::string s = "";
	std::string ini_file = FileFinder::Game().FindFile("Models/" + name);
	auto ini_stream = FileFinder::Game().OpenInputStream(ini_file, std::ios_base::in);

	Output::Debug("Load_OBJ");

	for (std::string line; getline(ini_stream, line); )
	{
		// Output::Debug("{}", line);
		if (line._Starts_with("v ")) {
			std::vector<std::string> v = split(line, " ");
			float x = std::stof(v[1]);
			float y = std::stof(v[2]);
			float z = std::stof(v[3]);
			vec3 v3 = { x,y,z };
			points3D.push_back(v3);
		}
		else if (line._Starts_with("f ")) {
			std::vector<std::string> v = split(line, " ");
			connection c;
			for (int i = 1; i < v.size(); i++) {
				auto vc = v[i];
				std::vector<std::string> f = split(vc, "/");
				if (i == 1) {
					c.a = std::stoi(f[0]) - 1;
				}
				else {
					c.b = std::stoi(f[0]) - 1;
					connections3D.push_back(c);
				}
			}
		}
	}
}

void Spriteset_MapDoom::Update() {
	//Output::Debug("Update");
	for (auto& p : points3D) {

		p.x -= centeroid.x;
		p.y -= centeroid.y;
		p.z -= centeroid.z;
		rotate(p, rotationX / 1000.0, rotationY / 1000.0, rotationZ / 1000.0);
		p.x += centeroid.x;
		p.y += centeroid.y;
		p.z += centeroid.z;


		pixel(p.x, p.y, p.z);
	}

	for (auto& c : connections3D) {
		line(points3D[c.a].x, points3D[c.a].y, points3D[c.b].x, points3D[c.b].y, points3D[c.a].z, points3D[c.b].z);
	}
	Show();
	points.clear();
}

void Spriteset_MapDoom::pixel(float x, float y, float z) {

	Point p = { x,y, false };
	if (z < 0) {
		p.upper = true;
	}

	points.emplace_back(p);
}
void Spriteset_MapDoom::line(float x1, float y1, float x2, float y2, float z1, float z2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	float length = std::sqrt(dx * dx + dy * dy);
	float angle = std::atan2(dy, dx);

	for (float i = 0; i < length; i++) {
		float zz = z1 + std::tanf(angle) * i;

		float f = i / length;
		zz = lerp(z1, z2, f);

		pixel(x1 + std::cosf(angle) * i, y1 + std::sin(angle) * i, zz);
	}
}

float Spriteset_MapDoom::lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void Spriteset_MapDoom::rotate(vec3& point, float x, float y, float z) {

	float rad = 0;

	rad = x;
	point.y = std::cos(rad) * point.y - std::sin(rad) * point.z;
	point.z = std::sin(rad) * point.y + std::cos(rad) * point.z;

	rad = y;
	point.x = std::cos(rad) * point.x + std::sin(rad) * point.z;
	point.z = -std::sin(rad) * point.x + std::cos(rad) * point.z;

	rad = z;
	point.x = std::cos(rad) * point.x - std::sin(rad) * point.y;
	point.y = std::sin(rad) * point.x + std::cos(rad) * point.y;

}

void Spriteset_MapDoom::Show() {

	sprite->Clear();
	spriteUpper->Clear();

	Rect r;
	Color c = Color(255,0,0,255);
	for (auto p : points) {
		r = Rect(p.x + displayX + Player::screen_width / 2 , p.y + displayY + Player::screen_height / 2, 1, 1);

		c = Color(100, 0, 0, 255);
		sprite->FillRect(r, c);

		if (p.upper) {
			c = Color(255, 0, 0, 255);
			spriteUpper->FillRect(r, c);
		}
	}
	

	//Output::Debug("Rendered");
}
