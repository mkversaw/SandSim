// g++ main.cpp `pkg-config --cflags --libs sdl2`
#include <SDL.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_EVERYTHING);
    
    SDL_Window* window = SDL_CreateWindow
    (
        "sandgame2",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, // initial window position
        600, 600,
        SDL_WINDOW_SHOWN
    );
    
    SDL_Renderer* renderer = SDL_CreateRenderer
    (
        window,
        -1, // initialize first driver that supports the requested flags
        SDL_RENDERER_ACCELERATED // use hardware acceleration
    );
    
    const unsigned int texWidth = 600;
    const unsigned int texHeight = 600;
    SDL_Texture* texture = SDL_CreateTexture
    (
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        texWidth, texHeight
    );
    
    std::vector< unsigned char > pixels(texWidth * texHeight * 4, 0);
    
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

            switch (event.type)
            {
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
                if (leftMouseButtonDown)
                {
                    const unsigned int x = m_x;
                    const unsigned int y = m_y;

                    const unsigned int offset = (texWidth * 4 * y) + x * 4;
                    pixels[offset + 0] = rand() % 256;        // b
                    pixels[offset + 1] = rand() % 256;        // g
                    pixels[offset + 2] = rand() % 256;        // r
                    pixels[offset + 3] = SDL_ALPHA_OPAQUE;    // a
                }
                break;
            }

        }
    
        SDL_UpdateTexture
        (
            texture,
            NULL,
            pixels.data(),
            texWidth * 4
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