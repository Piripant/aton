#include "graphics.h"

#include <iostream>
#include <chrono>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <graphics/imgui_impl_sdl_gl3.h>
#include <automata/world.h>

using namespace std;

int lpen_state = 0;
int rpen_state = 0;

int view_x, view_y = 0;

int old_mouse_x, old_mouse_y = 0;
int mouse_x, mouse_y = 0;
bool left_mouse_down = false;
bool right_mouse_down = false;
bool shift_pressed = false;

unsigned int scale = 25;
float cell_size = 1;

float back_color[3] = {1, 1, 1};

unsigned int generation = 0;

bool update_view = true;

std::string get_file_name(char* file_name, unsigned int len) {
	std::string string_name;
	for (unsigned int i = 0; i < len; i++) {
		if (file_name[i] != ' ') {
			string_name += file_name[i];
		}
	}
	return string_name;
}

void put_in_range(int& num, int min, int max) {
	if (num < min) {
		num = min;
	}
	else if (num > max) {
		num = max;
	}
}


bool run = false; // The simulation is running

inline void simulation_gui() {
	ImGui::SetNextWindowSize(ImVec2(410, 100), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Simulation");
	ImGui::Text("Running: %d", run);
	ImGui::Text("Generation: %d", generation);
	ImGui::Checkbox("Update view", (bool*)&update_view);

	if (ImGui::Button("Step")) {
		World::script->Step();
		generation++;
	}
	if (ImGui::Button("Start")) {
		run = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop") && run) {
		run = false;
	}
	ImGui::End();
}

bool show_world_dialog = false;
inline void new_world_dialog() {
	static int nw_width = 1;
	static int nw_height = 1;
	static int nw_state;
	static bool *p_open;
	
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoSavedSettings;

	ImGui::SetNextWindowSize(ImVec2(400, 125), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("New world", p_open, window_flags);

	ImGui::InputInt("Width", (int*)&nw_width);
	ImGui::InputInt("Height", (int*)&nw_height);
	ImGui::InputInt("Default state", (int*)&nw_state);
	put_in_range(nw_width, 1, 1000000000);
	put_in_range(nw_height, 1, 1000000000);
	put_in_range(nw_state, 0, 255);

	if (ImGui::Button("Create")) {
		World::Resize((uint8_t)nw_state, nw_width, nw_height);
		generation = 0;
		show_world_dialog = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		show_world_dialog = false;
	}

	ImGui::End();
}

inline void world_settings_gui(int& selected_state, float *colors) {
	static char world_file_name[30] = "";
	static char script_file_name[30] = "";

	ImGui::SetNextWindowSize(ImVec2(500,100), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("World");

	ImGui::Text("Overing over cell: (%d, %d)", (mouse_x-view_x)/(int)scale, (view_y-mouse_y)/(int)scale);
	ImGui::Text("Zoom: %d", scale);

	if (ImGui::CollapsingHeader("File")) {
		if (ImGui::Button("New world") && !show_world_dialog) {
			show_world_dialog = true;
		}

		ImGui::Separator();
		ImGui::InputText("World file name", world_file_name, 30);
		if (ImGui::Button("Save file")) {
			World::SaveWorld(get_file_name(world_file_name, 30));
		}

		if (ImGui::Button("Load file")) {
			World::LoadWorld(get_file_name(world_file_name, 30));

			// To set the colors to the ones of the world just loaded
			colors[0] = (float)World::colors[selected_state][0]/255;
			colors[1] = (float)World::colors[selected_state][1]/255;
			colors[2] = (float)World::colors[selected_state][2]/255;
		}
	}

	if (ImGui::CollapsingHeader("Graphics")) {
		ImGui::ColorEdit3("Background color", (float*)&back_color);
		ImGui::SliderFloat("Cell size", &cell_size, 0.0f, 1.0f);
		
		ImGui::Separator();
		
		ImGui::InputInt("Selected state", &selected_state);
		put_in_range(selected_state, 0, 255);

		// To set the colors to the ones of the new state selected
		colors[0] = (float)World::colors[selected_state][0]/255;
		colors[1] = (float)World::colors[selected_state][1]/255;
		colors[2] = (float)World::colors[selected_state][2]/255;

		ImGui::ColorEdit3("State color", colors);
	}

	if (ImGui::CollapsingHeader("Scripts")) {
		ImGui::InputText("Script path", script_file_name, 30);
		if (ImGui::Button("Load file")) {
			std::cout << get_file_name(script_file_name, 30) << std::endl;
			World::LoadScript(get_file_name(script_file_name, 30));
		}
	}
	
	ImGui::End();
}

inline void pen_gui() {
	ImGui::SetNextWindowSize(ImVec2(500,100), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Pen settings");
	ImGui::InputInt("Left click state", &lpen_state);
	put_in_range(lpen_state, 0, 255);
	ImGui::InputInt("Right click state", &rpen_state);
	put_in_range(rpen_state, 0, 255);
	ImGui::End();
}

void update_gui(SDL_Window *window) {
	static int selected_state = 0;
	static float colors[3] = {0, 0, 0};

	ImGui_ImplSdlGL3_NewFrame(window);
	world_settings_gui(selected_state, (float*)colors);
	simulation_gui();
	pen_gui();

	if (show_world_dialog) {
		new_world_dialog();
	}

	World::colors[selected_state][0] = colors[0]*255;
	World::colors[selected_state][1] = colors[1]*255;
	World::colors[selected_state][2] = colors[2]*255;
}

void input_loop(SDL_Event& event) {
	ImGui_ImplSdlGL3_ProcessEvent(&event);

	switch (event.type) {
		if (!ImGui::GetIO().WantCaptureMouse) {
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT) {
					left_mouse_down = true;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT) {
					right_mouse_down = true;
				}
			break;
			
			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT) {
					left_mouse_down = false;
				}
				else if (event.button.button == SDL_BUTTON_RIGHT) {
					right_mouse_down = false;
				}
			break;
			
			case SDL_MOUSEWHEEL:
				if (event.wheel.y > 0)
					scale += 1;
				if (event.wheel.y < 0 && scale > 1)
					scale -= 1;
			break;
		}

		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_LSHIFT) {
				shift_pressed = true;
			}
		break;

		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LSHIFT) {
				shift_pressed = false;
			}
		break;
	}

	SDL_GetMouseState(&mouse_x, &mouse_y);

	if (left_mouse_down) {
		World::GetCurrentTable().SafeSetCell(lpen_state, (mouse_x-view_x)/scale, (view_y-mouse_y)/scale);
	}
	if (right_mouse_down) {
		if (!shift_pressed) {
			World::GetCurrentTable().SafeSetCell(rpen_state, (mouse_x-view_x)/scale, (view_y-mouse_y)/scale);
		}
		else {
			int delta_x = mouse_x - old_mouse_x;
			int delta_y = mouse_y - old_mouse_y;

			view_x += delta_x;
			view_y += delta_y;
		}
	}

	old_mouse_x = mouse_x;
	old_mouse_y = mouse_y;
}

chrono::time_point<chrono::system_clock> start_frame;
chrono::duration<double> elapsed;
void draw_world()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_DisplayMode current;
	SDL_GetCurrentDisplayMode(0, &current);
	SDL_Window *window = SDL_CreateWindow("Aton", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	
	SDL_Renderer* renderer = NULL;
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

	glewInit();

	ImGui_ImplSdlGL3_Init(window);

	SDL_Event event;

	bool quit = false;
	while(!quit) {
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)	{
				quit = true;
			}

			input_loop(event);
		}
		
    	start_frame = chrono::system_clock::now();
		do {
			if (run) {
				World::script->Step();
				generation++;
			}

			elapsed = chrono::system_clock::now()-start_frame;
		} while(elapsed.count() < 0.016);

		unsigned int back_color_8[3] = {(unsigned int)(back_color[0]*255), (unsigned int)(back_color[1]*255), (unsigned int)(back_color[2]*255)};
		SDL_SetRenderDrawColor(renderer, back_color_8[0], back_color_8[1], back_color_8[2], 255);
		SDL_RenderClear(renderer);
		if (update_view) {
			Table& table = World::GetCurrentTable();

			int display_x = ImGui::GetIO().DisplaySize.x;
			int display_y = ImGui::GetIO().DisplaySize.y;

			for (unsigned int x = 0; x < table.width; x++) {
				for (unsigned int y = 0; y < table.height; y++) {
					uint8_t& cell_state = table.GetCellState(x, y);
					
					#define cell_color_r World::colors[cell_state][0]
					#define cell_color_g World::colors[cell_state][1]
					#define cell_color_b World::colors[cell_state][2]

					int x_pos = x*scale + view_x;
					int y_pos = -(y+1)*scale + view_y;

					if (cell_color_r == back_color_8[0] && cell_color_g == back_color_8[1] && cell_color_b == back_color_8[2]) {
						continue;
					}
					if (x_pos < 0 || x_pos > display_x) {
						continue;
					}
					if (y_pos < 0 || y_pos > display_y) {
						continue;
					}

					SDL_Rect r;
					r.x = x_pos;
					r.y = y_pos;
					r.w = scale*cell_size;
					r.h = scale*cell_size;

					SDL_SetRenderDrawColor(renderer, cell_color_r, cell_color_g, cell_color_b, 255);
					SDL_RenderFillRect(renderer, &r);
				}
			}
		}

		update_gui(window);
		ImGui::Render();
		SDL_RenderPresent(renderer);
		
		SDL_GL_SwapWindow(window);
	}

	ImGui_ImplSdlGL3_Shutdown();
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();
}