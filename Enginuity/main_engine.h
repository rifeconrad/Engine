#pragma once

#include <SPEngine2D/sprite.h>
#include <SPEngine2D/GLSL_program.h>
#include <SPEngine2D/GLTexture.h>
#include <SPEngine2D/window.h>
#include <SPEngine2D/camera_2D.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <vector>

enum class EngineState {PLAY = 0, EXIT};
class MainEngine
{
public:
	MainEngine();
	~MainEngine();

	void run();

private:
	SPEngine2D::Window window;

	int screen_width;
	int screen_height;

	EngineState current_state;

	void init();
	void initShaders();
	void handleInput();
	void draw();
	void engineLoop();

	void calculateFPS();

	const float MAX_FPS;
	float fps;
	float frame_time;

	std::vector<SPEngine2D::Sprite*> sprites;

	SPEngine2D::GLSLProgram color_program;

	SPEngine2D::Camera2D camera;

	float time;
};

