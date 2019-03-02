#include "main_engine.h"

#include "SPEngine2D/eng_errors.h"
#include "SPEngine2D/SPEngine2D.h"

#include <iostream>
#include <string>

MainEngine::MainEngine() : screen_width(1024), screen_height(768), current_state(EngineState::PLAY), time(0), MAX_FPS(60.f)
{
	camera.init(this->screen_width, this->screen_height);
}

MainEngine::~MainEngine()
{
}

void MainEngine::init()
{
	SPEngine2D::init();

	this->window.create("OpenGL Test One", screen_width, screen_height, 0);

	this->initShaders();
}

void MainEngine::initShaders()
{
	this->color_program.CompileShaders("shaders/colorShading.vert", "shaders/colorShading.frag");
	this->color_program.addAttribute("vertexPosition");
	this->color_program.addAttribute("vertexColor");
	this->color_program.addAttribute("vertexUV");
	this->color_program.LinkShaders();
}

void MainEngine::run()
{
	this->init();

	this->sprites.push_back(new SPEngine2D::Sprite());
	this->sprites.back()->init(0.f, 0.f, this->screen_width / 2, this->screen_width / 2, "./textures/jimmy_jump_pack/PNG/Enemys/Enemy_Broccoli1.png");
	this->sprites.push_back(new SPEngine2D::Sprite());
	this->sprites.back()->init(this->screen_width / 2, 0.f, this->screen_width / 2, this->screen_width / 2, "./textures/jimmy_jump_pack/PNG/Enemys/Enemy_Broccoli1.png");

	//this->player_texture = ImageLoader::loadPNG("./textures/jimmy_jump_pack/PNG/Enemys/Enemy_Broccoli1.png");

	this->engineLoop();
}

void MainEngine::engineLoop()
{
	while (this->current_state != EngineState::EXIT)
	{
		// used for frame time measuring
		float start_ticks = SDL_GetTicks();

		this->handleInput();

		this->time += 0.1;

		this->camera.update();

		this->draw();

		this->calculateFPS();




		// limit FPS display
		static int frame_counter = 0;
		frame_counter++;
		if (frame_counter == 10)
		{
			std::cerr << this->fps << "\n";
			frame_counter = 0;
		}

		float actual_frame_time = SDL_GetTicks() - start_ticks;

		// FPS limiter
		// if our frame was quicker than intended
		float desired_frame_time = 1000.f / this->MAX_FPS;
		if (desired_frame_time > actual_frame_time)
		{
			// delay the frame to our desired frame time
			SDL_Delay(desired_frame_time - actual_frame_time);
		}
	}
}

void MainEngine::calculateFPS()
{
	// # of frames we want to average
	static const int NUM_SAMPLES = 10;
	// a collection of the frame times
	static float frame_times[NUM_SAMPLES];
	// the current frame time
	static int curr_frame = 0;

	// initialize the ticks for this frame
	static float prev_ticks = SDL_GetTicks();

	// obtain the current ticks
	float curr_ticks;
	curr_ticks = SDL_GetTicks();

	// how long has this frame taken?
	this->frame_time = curr_ticks - prev_ticks;

	// do a circular storage of the frames
	frame_times[curr_frame % NUM_SAMPLES] = this->frame_time;

	// set our previous ticks to our current ticks for the next call to this method
	prev_ticks = curr_ticks;

	// increment the # of frames we have had
	curr_frame++;

	// determine which frame we are on now
	int count;
	if (curr_frame < NUM_SAMPLES)
		count = curr_frame; // if we are less than 10 frames, we need to account for that
	else
		count = NUM_SAMPLES; // anything more than 10 frames

	// loop through each stored frame and sum them up
	float avg_frame_time = 0;
	for (int i = 0; i < count; i++)
	{
		avg_frame_time += frame_times[i];
	}

	// calculate the average of the summed up frames
	avg_frame_time /= count;

	// check if the avg_frame_time is equal to 0 (avoids division by 0)
	if (avg_frame_time > 0)
	{
		// if not, we have our FPS!
		this->fps = 1000.f / avg_frame_time;
	}
	else
	{
		// if so, print off a generic value
		this->fps = 60.f;
	}
}

void MainEngine::handleInput()
{
	SDL_Event event;

	const float camera_speed = 20.f;
	const float scale_speed = 0.1f;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			this->current_state = EngineState::EXIT;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				this->camera.setPosition(this->camera.getPosition() + glm::vec2(0.f, camera_speed));
				break;
			case SDLK_s:
				this->camera.setPosition(this->camera.getPosition() + glm::vec2(0.f, -camera_speed));
				break;
			case SDLK_a:
				this->camera.setPosition(this->camera.getPosition() + glm::vec2(-camera_speed, 0.f));
				break;
			case SDLK_d:
				this->camera.setPosition(this->camera.getPosition() + glm::vec2(camera_speed, 0.f));
				break;
			case SDLK_q:
				this->camera.setScale(this->camera.getScale() + scale_speed);
				break;
			case SDLK_e:
				this->camera.setScale(this->camera.getScale() - scale_speed);
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			//std::cerr << event.motion.x << ", " << event.motion.y << "\n";
			break;
		}
	}
}

void MainEngine::draw()
{
	glClearDepth(1.0);
	// clear the color buffer and clear the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	this->color_program.use();

	// glActiveTexture allows us to render multiple textures
	// GL_TEXTURE0 would be our first texture, then
	// GL_TEXTURE1 would be our second, etc.
	glActiveTexture(GL_TEXTURE0);

	GLint textureLocation = this->color_program.getUniformLocation("mySampler");
	glUniform1i(textureLocation, 0); // 0 references GL_TEXTURE0

	GLuint timeLocation = this->color_program.getUniformLocation("time");
	glUniform1f(timeLocation, this->time);

	// set camera matrix
	GLuint pLocation = this->color_program.getUniformLocation("p");
	glm::mat4 camera_matrix = this->camera.getCameraMatrix();
	glUniformMatrix4fv(pLocation, 1, GL_FALSE, &(camera_matrix[0][0]));

	for (int i = 0; i < this->sprites.size(); i++)
		this->sprites[i]->draw();

	glBindTexture(GL_TEXTURE_2D, 0);

	this->color_program.unuse();

	// adds everything drawn to non-shown window to the screen
	this->window.swapBuffer();
}
