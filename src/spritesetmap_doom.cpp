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

	//points3D = {};
	//connections3D = {};
	//surfaces = { { {16,16}, {16,24},{32,32},{16,16} ,{48,24} }};
	//surfaces = { {{41,5} ,{31,82}, {37,82}}, {{41,5} ,{31,82},{46,82}}, {{27,83}, {48,83}, {27,92}, {48,92}} };

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

	Output::Debug("Load_OBJ");

	points3D = {};
	connections3D = {};

	surfaces = {};

	Output::Debug(".mtl");

	std::string s = "";
	std::string ini_file = FileFinder::Game().FindFile("Models/" + name + ".mtl");
	auto ini_stream = FileFinder::Game().OpenInputStream(ini_file, std::ios_base::in);

	std::map<std::string, Color> colors;
	std::string material_name = "";

	for (std::string line; getline(ini_stream, line); )
	{
		// Output::Debug("{}", line);
		if (line._Starts_with("newmtl ")) {
			material_name = line.substr(7,line.length() - 7);
		} else if (line._Starts_with("Kd ")) {
			std::vector<std::string> v = split(line, " ");
			int r = std::stof(v[1]) * 255;
			int g = std::stof(v[2]) * 255;
			int b = std::stof(v[3]) * 255;
			colors[material_name] = Color(r, g, b, 255);

			Output::Debug(" {} {} {} {}", material_name, r, g, b);
		}
	}

	Output::Debug(".obj");

	std::vector <Point> pointsNoMaterial;
	s = "";
	ini_file = FileFinder::Game().FindFile("Models/" + name + ".obj");
	ini_stream = FileFinder::Game().OpenInputStream(ini_file, std::ios_base::in);
	Color color;

	for (std::string line; getline(ini_stream, line); )
	{
		// Output::Debug("{}", line);
		if (line._Starts_with("v ")) {
			std::vector<std::string> v = split(line, " ");
			float x = std::stof(v[1]);
			float y = std::stof(v[2]);
			float z = std::stof(v[3]);

			Point v3 = { x,y,z, false };
			points3D.push_back(v3);
			/*pointsNoMaterial.push_back(v3);*/
		}
		// Material
		else if (line._Starts_with("usemtl ")) {
			material_name = line.substr(7, line.length() - 7);
			Output::Debug("{}", material_name);
			color = colors[material_name];

			Output::Debug(" {} {} {}", color.red, color.green, color.blue);

			//for (auto p : pointsNoMaterial) {
			//	p.color = color;
			//	points3D.push_back(p);
			//}
			//pointsNoMaterial.clear();

		}
		// Surface
		else if (line._Starts_with("f ")) {
			std::vector<std::string> v = split(line, " ");
			std::vector<Point> s;
			Point p;
			p.color = color;
			for (int i = 1; i < v.size(); i++) {
				auto vc = v[i];
				std::vector<std::string> f = split(vc, "/");

				if (i == 1) {
					p.y = std::stof(f[0]) - 1;
				}
				else if (i == v.size() - 1) {
					p.x = p.y;
					p.y = std::stof(f[0]) - 1;
					s.push_back(p);
					p.x = p.y;
					p.y = s[0].x;
					s.push_back(p);
				}
				else {
					p.x = p.y;
					p.y = std::stof(f[0]) - 1;
					s.push_back(p);
				}

			}

			connection c;
			int cc;
			for (int i = 1; i < v.size(); i++) {
				auto vc = v[i];
				std::vector<std::string> f = split(vc, "/");
				if (i == 1) {
					c.a = std::stoi(f[0]) - 1;
					cc = c.a;
				}
				else {
					c.b = std::stoi(f[0]) - 1;
					connections3D.push_back(c);
					c.a = c.b;
					if (i == v.size() - 1) {
						c.b = cc;
						connections3D.push_back(c);
					}
				}
			}

			surfaces.push_back(s);
			
		}
		 // => Wireframe
		/*else if (line._Starts_with("f ")) {
			std::vector<std::string> v = split(line, " ");
			connection c;
			int cc;
			for (int i = 1; i < v.size(); i++) {
				auto vc = v[i];
				std::vector<std::string> f = split(vc, "/");
				if (i == 1) {
					c.a = std::stoi(f[0]) - 1;
					cc = c.a;
				}
				else {
					c.b = std::stoi(f[0]) - 1;
					connections3D.push_back(c);
					c.a = c.b;
					if (i == v.size() - 1) {
						c.b = cc;
						connections3D.push_back(c);
					}
				}
			}
		}*/
	}

	//for (auto p : pointsNoMaterial) {
	//	p.color = color;
	//	points3D.push_back(p);
	//}
	//pointsNoMaterial.clear();
}

void Spriteset_MapDoom::Update() {
	//Output::Debug("Update");

	int refresh_rate = 1;

	for (auto& p : points3D) {

		p.x -= centeroid.x;
		p.y -= centeroid.y;
		p.z -= centeroid.z;
		rotate(p, rotationX / 1000.0, rotationY / 1000.0, rotationZ / 1000.0);
		p.x += centeroid.x;
		p.y += centeroid.y;
		p.z += centeroid.z;

		//if (timer % refresh_rate == 1 || refresh_rate == 1)
			//pixel(p.x, p.y, p.z, p.color);
	}

	if (timer % refresh_rate == 1 || refresh_rate == 1) {

		for (auto& c : connections3D) {
			// line(points3D[c.a], points3D[c.b]);
		}
		for (auto s : surfaces) {

			std::vector<Point> s2;
			for (auto p : s) {
				Point p2 = { points3D[p.x].x, points3D[p.x].y, points3D[p.x].z, points3D[p.x].upper, p.color};
				s2.push_back(p2);

				p2 = { points3D[p.y].x, points3D[p.y].y, points3D[p.y].z, points3D[p.y].upper, p.color };
				s2.push_back(p2);

				// Output::Debug(" {} {} {}", p2.color.red, p2.color.green , p2.color.blue);
			}
			sortPoints(s2);
			drawPolygon(s2);

		}

		Show();

	}
	timer++;
	points.clear();
}

void Spriteset_MapDoom::pixel(float x, float y, float z, Color c) {

	Point p = { x, y, z, false, c};
	if (z < 0) {
		p.upper = true;
	}

	points.emplace_back(p);
}

void Spriteset_MapDoom::line(Point p1, Point p2) {

	float x1, y1, x2, y2, z1, z2;
	x1 = p1.x;
	y1 = p1.y;
	x2 = p2.x;
	y2 = p2.y;
	z1 = p1.z;
	z2 = p2.z;

	float dx = x2 - x1;
	float dy = y2 - y1;
	float length = std::sqrt(dx * dx + dy * dy);
	float angle = std::atan2(dy, dx);

	for (float i = 0; i < length; i++) {
		float zz = z1 + std::tanf(angle) * i;

		float f = i / length;
		zz = lerp(z1, z2, f);

		pixel(x1 + std::cosf(angle) * i, y1 + std::sin(angle) * i, zz, p1.color);
	}
}

// Fonction pour calculer le centroïde du polygone
Spriteset_MapDoom::Point Spriteset_MapDoom::computeCentroid(const std::vector<Spriteset_MapDoom::Point>& points) {
	int x_sum = 0, y_sum = 0;
	for (const auto& p : points) {
		x_sum += p.x;
		y_sum += p.y;
	}
	Point p;
	p.x = x_sum / static_cast<int>(points.size());
	p.y = y_sum / static_cast<int>(points.size());
	return p;
}

// Fonction pour trier les points en fonction de leur angle par rapport au centroïde
void Spriteset_MapDoom::sortPoints(std::vector<Spriteset_MapDoom::Point>& points) {
	Point centroid = computeCentroid(points);
	std::sort(points.begin(), points.end(), [centroid](const Point& a, const Point& b) {
		return std::atan2(a.y - centroid.y, a.x - centroid.x) < std::atan2(b.y - centroid.y, b.x - centroid.x);
		});
}

void Spriteset_MapDoom::drawPolygon(std::vector<Spriteset_MapDoom::Point> vertices) {

	if (vertices.size() < 3) return;  // Pas un polygone valide

	Color color = Color(255, 255, 0, 255);

	color = vertices[0].color;

	int z = 0;
	float z_sum = 0.0;

	// Additionner toutes les valeurs de Z
	for (const auto& point : vertices) {
		z_sum += point.z;
	}

	// Calculer la moyenne
	z = z_sum / vertices.size();

	// Trouver les bornes verticales (y_min et y_max)
	int y_min = vertices[0].y;
	int y_max = vertices[0].y;
	for (const auto& p : vertices) {
		y_min = std::min(y_min, (int) p.y);
		y_max = std::max(y_max, (int) p.y);
	}

	// Pour chaque ligne horizontale (scanline), déterminer les vertices d'intersection
	for (int y = y_min; y <= y_max; ++y) {
		std::vector<int> intersections;

		// Trouver les vertices d'intersection entre la ligne de balayage et les arêtes du polygone
		for (size_t i = 0; i < vertices.size(); ++i) {
			Point p1 = vertices[i];
			Point p2 = vertices[(i + 1) % vertices.size()];  // Cycle pour connecter le dernier point au premier

			// Vérifier si l'arête coupe la ligne de balayage
			if ((p1.y <= y && p2.y > y) || (p2.y <= y && p1.y > y)) {
				// Calculer le point d'intersection en x par interpolation linéaire
				int x = p1.x + (y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
				intersections.push_back(x);
			}
		}

		// Trier les intersections pour dessiner les segments horizontaux
		std::sort(intersections.begin(), intersections.end());

		// Remplir les pixels entre chaque paire de vertices d'intersection
		for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
			int x_start = intersections[i];
			int x_end = intersections[i + 1];
			for (int x = x_start; x <= x_end; ++x) {
				pixel(x, y, z, color);  // Dessiner les pixels dans l'intervalle
			}
		}
	}
}


float Spriteset_MapDoom::lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void Spriteset_MapDoom::rotate(Point& point, float x, float y, float z) {

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

bool Spriteset_MapDoom::comparePoints(const Point& p1, const Point& p2) {
	return p1.z < p2.z;
}

void Spriteset_MapDoom::Show() {

	sprite->Clear();
	spriteUpper->Clear();

	float zmin = 0;
	float zmax = points[0].z;
	for (const auto& p : points) {
		//Output::Debug(" {}", p.z);
		zmin = std::max(zmin, p.z);
		zmax = std::min(zmax, p.z);
	}

	std::sort(points.begin(), points.end(), std::greater<Point>());


	Rect r;
	Color c = Color(255,0,0,255);
	for (auto p : points) {
		r = Rect(p.x + displayX + Player::screen_width / 2 , Player::screen_height - (p.y + displayY + Player::screen_height / 2), 1, 1);

		int mult = ((p.z - zmin) / (zmax - zmin)) * 100;

		int red, green, blue;
		red = p.color.red;
		green = p.color.green;
		blue = p.color.blue;

		//Output::Debug(" {} {} {}", red, green, blue);

		c = Color(red * mult / 100, green * mult / 100, blue * mult / 100, 255);
		//c = p.color;
		sprite->FillRect(r, c);

		if (p.upper) {
			c = Color(255, 0, 0, 255);
			spriteUpper->FillRect(r, c);
		}
	}
	

	//Output::Debug("Rendered");
}
