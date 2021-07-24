// g++ main.cpp `pkg-config --cflags --libs sdl2`
#include <SDL.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>

bool inBounds(int x, int y);

const unsigned int width = 600;
const unsigned int height = 600;

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow
    (
        "sandgame2",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, // initial window position
        width, height,
        SDL_WINDOW_SHOWN
    );
    
    SDL_Renderer* renderer = SDL_CreateRenderer
    (
        window,
        -1, // initialize first driver that supports the requested flags
        SDL_RENDERER_ACCELERATED // use hardware acceleration
    );
    
    SDL_Texture* texture = SDL_CreateTexture
    (
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );
    
    std::vector< unsigned char > pixels(width * height * 4, 0);
    const int br_size = 9;

    SDL_Event event;
    bool running = true;
    
    unsigned int frames = 0;
    Uint64 start = SDL_GetPerformanceCounter();
    
    bool leftMouseButtonDown = false;

    while (running) {
    
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
    
        while (SDL_PollEvent(&event)) {
            if ((SDL_QUIT == event.type) ||
                (SDL_KEYDOWN == event.type && SDL_SCANCODE_ESCAPE == event.key.keysym.scancode)) {
                running = false;
                break;
            }

            int m_x, m_y;
            Uint32 mouse_state = SDL_GetMouseState(&m_x, &m_y); // get mouse position

            switch (event.type) {
            case SDL_QUIT:
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                    leftMouseButtonDown = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                    leftMouseButtonDown = true;
            case SDL_MOUSEMOTION:
                if (leftMouseButtonDown) {
                    unsigned char col_r = rand() % 256;
                    unsigned char col_g = rand() % 256;
                    unsigned char col_b = rand() % 256;
                    unsigned char col_o = SDL_ALPHA_OPAQUE;

                    for (int br_x = m_x - br_size; br_x < m_x + br_size; br_x++) {
                        for (int br_y = m_y - br_size; br_y < m_y + br_size; br_y++) {
                            if(inBounds(br_x,br_y)) {
                                const unsigned int offset = (width * 4 * br_y) + br_x * 4;
                                pixels[offset + 0] = col_b;          // b
                                pixels[offset + 1] = col_g;          // g
                                pixels[offset + 2] = col_r;          // r
                                pixels[offset + 3] = col_o;          // o
                            }

                        }
                    }
                }
                break;
            }

        }
    
        SDL_UpdateTexture
        (
            texture,
            NULL,
            pixels.data(),
            width * 4
        );
        
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    
        frames++;
        const Uint64 end = SDL_GetPerformanceCounter();
        const static Uint64 freq = SDL_GetPerformanceFrequency();
        const double seconds = (end - start) / static_cast<double>(freq);
        if (seconds > 2.0)
        {
            std::cout
                << frames << " frames in "
                << std::setprecision(1) << std::fixed << seconds << " seconds = "
                << std::setprecision(1) << std::fixed << frames / seconds << " FPS ("
                << std::setprecision(3) << std::fixed << (seconds * 1000.0) / frames << " ms/frame)"
                << std::endl;
            start = end;
            frames = 0;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

bool inBounds(int x, int y) {
    return x >= 0 && y >= 0 && x < 600 && y < 600;
}