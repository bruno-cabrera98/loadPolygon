
#include <SDL.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

class vec3 {
public:
	enum class vecType {
		V_COLOR,
		V_POS,
		V_NORMAL
	};

	vec3(float x, float y, float z, vecType type)
		: x(x), y(y), z(z), type(type) {}

	float getX() { return x; }
	float getY() { return y; }
	float getZ() { return z; }
	vecType getType() { return type; }

	void setX(float x) { x = x; }
	void setY(float y) { y = y; }
	void setZ(float z) { z = z; }
	void setType(vecType type) { type = type; }

private:
	float x, y, z;
	vecType type;
};

/*
	Retorna vector con vec3 de archivo polygons.txt
*/
std::vector<vec3> parseFile(const char * name) {
	std::vector<vec3> result;
	std::ifstream input;
	input.open(name, std::ifstream::in);
	if (input.fail()) {
		std::cout << "It failed\n" << name;
	}
	float x, y, z;
	char t;
	while (input >> t >> x >> y >> z) {
		vec3::vecType type;
		switch (t) {
		case 'C':
			type = vec3::vecType::V_COLOR;
			break;
		case 'V':
			type = vec3::vecType::V_POS;
			break;
		default:
			type = vec3::vecType::V_NORMAL;
			break;
		}
		result.push_back(vec3(x, y, z, type));
	}
	input.close();
	return result ;
}

/*
	Dibuja los poligonos (Necesita de un contexto opengl)
*/
void displayPolygons(std::vector<vec3> polygons) {
	int vtxCount = 0;
	for (vec3 v : polygons) {
		switch (v.getType()) {
		case vec3::vecType::V_COLOR:
			glColor3f(v.getX(), v.getY(), v.getZ());
			break;
		case vec3::vecType::V_POS:
			if (vtxCount == 0) glBegin(GL_TRIANGLES);
			glVertex3f(v.getX(), v.getY(), v.getZ());
			vtxCount = (vtxCount + 1)%3;
			if (vtxCount == 0) glEnd();
			break;
		default:
			glNormal3f(v.getX(), v.getY(), v.getZ());
			break;
		}
	}
}



int main(int argc, char* argv[]) {
	std::vector<vec3> polygons;
	polygons = parseFile("polygons.txt");
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "There was an error\n";
		exit(1);
	}

	SDL_Window* win = SDL_CreateWindow("movement",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_GLContext context = SDL_GL_CreateContext(win);

	glClearColor(0.0, 0.0, 0.0, 1);

	glMatrixMode(GL_PROJECTION);
	gluPerspective(45, 640 / 480.f, 0.1, 100);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);

	SDL_Event event;

	bool full = false;

	float angle = 0;
	const double angPerMil = (0.2 * 360.0) / 1000; // Angulo por milisegundo (Una vueta cada 5 segundos)
	double delta = 0;
	Uint64 now = SDL_GetPerformanceCounter();
	Uint64 last;

	

	do {
		last = now;
		now = SDL_GetPerformanceCounter();

		delta = ((now - last) * 1000 / (double)SDL_GetPerformanceFrequency());
		angle = fmod((angle + angPerMil * delta), 360);

		glMatrixMode(GL_MODELVIEW);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();
		gluLookAt(0, 0, 0, 0, 0, -6, 0, 1, 0);

		glTranslatef(0, 0, -6);

		glRotatef(angle, 0, 1, 0);
		displayPolygons(polygons);


		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					return 0;
					break;
				case SDLK_F11:
					glMatrixMode(GL_PROJECTION);
					if (full) {
						SDL_SetWindowFullscreen(win, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
						full = false;
					}
					else {
						SDL_SetWindowFullscreen(win, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
						full = true;
						std::cout << "FULL";
					}
					break;
				case SDLK_SPACE:
					polygons = parseFile("polygons.txt");
				default:
					break;
				}
			}
		}
		SDL_GL_SwapWindow(win);
	} while (true);

	return 0;

}



