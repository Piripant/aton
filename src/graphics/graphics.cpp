#include <SDL2/SDL.h>
#include "../automata/world.h"

void draw_world(World& world)
{
    SDL_Window* window = NULL;
    window = SDL_CreateWindow("Atomata", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;

    bool quit = false;
    while(!quit) {
        Table& table = world.GetCurrentTable();

        SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
        SDL_RenderClear(renderer);

        for (unsigned int x = 0; x < table.width; x++) {
            for (unsigned int y = 0; y < table.height; y++) {
                uint8_t& cell_state = table.GetCellState(x, y);
                
                SDL_Rect r;
                r.x = x*25;
                r.y = y*25;
                r.w = 25;
                r.h = 25;

                SDL_SetRenderDrawColor(renderer, world.colors[cell_state][0], world.colors[cell_state][1], world.colors[cell_state][2], 255);
                SDL_RenderFillRect(renderer, &r);
            }
        }

        SDL_RenderPresent(renderer);

        while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        SDL_Delay(60);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}