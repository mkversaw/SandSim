// g++ main.cpp `pkg-config --cflags --libs sdl2`
#include <SDL.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstring>

struct pix {
	unsigned char col_r;
	unsigned char col_g;
	unsigned char col_b;
	unsigned char col_o;

	bool empty;
	bool visited;

	pix() {
		col_r = 0;
		col_g = 0;
		col_b = 0;
		col_o = 0;

		empty = true;
		visited = false;
	}

};

bool inBounds(int x, int y);
bool isFree(int x, int y);

const unsigned int width = 600;
const unsigned int height = 600;

std::vector< unsigned char > pixels(width* height * 4, 0);
std::vector<std::vector<pix>> pixs(width, std::vector<pix>(height));



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
   
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			pixs[i].push_back(pix());
		}
	}

	const int br_size = 9;

	SDL_Event event;
	bool running = true;
	
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
							if (inBounds(br_x, br_y) && pixs[br_x][br_y].empty) {
								const unsigned int offset = (width * 4 * br_y) + br_x * 4;
								pixels[offset + 0] = col_b;          // b
								pixels[offset + 1] = col_g;          // g
								pixels[offset + 2] = col_r;          // r
								pixels[offset + 3] = col_o;          // o

								pixs[br_x][br_y].empty = false;
								pixs[br_x][br_y].col_r = col_r;
								pixs[br_x][br_y].col_g = col_g;
								pixs[br_x][br_y].col_b = col_b;
								pixs[br_x][br_y].col_o = col_o;

							}
						}
					}

						
					
				}
				break;
			}

		}
		
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (!pixs[i][j].visited) { // not visited yet
					pixs[i][j].visited = true;

					bool down = isFree(i, j + 1);
					bool left = isFree(i - 1, j);
					bool right = isFree(i + 1, j);

					if (inBounds(i,j + 1)) { // is a legal position
						if (pixs[i][j].empty == false && pixs[i][j + 1].empty == true) { // is not occupied
							pixs[i][j + 1] = pixs[i][j];		
							pixs[i][j].empty = true;
							pixs[i][j].col_r = 0;          // b
							pixs[i][j].col_b = 0;          // g
							pixs[i][j].col_g = 0;          // r
							pixs[i][j].col_o = 255;        // o
						}
					}
				}
				pixs[i][j].visited = false;
			}
		}

		for (int i = 0; i < width; i++) { // update the actual drawn pixels
			for (int j = 0; j < height; j++) {
				const unsigned int offset = (width * 4 * j) + i * 4;
				pixels[offset + 0] = pixs[i][j].col_b;          // b
				pixels[offset + 1] = pixs[i][j].col_g;          // g
				pixels[offset + 2] = pixs[i][j].col_r;          // r
				pixels[offset + 3] = pixs[i][j].col_o;          // o
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

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

bool isFree(int x, int y) {
	return inBounds(x, y) && pixs[x][y].empty;
}

bool inBounds(int x, int y) {
	return x >= 0 && y >= 0 && x < 600 && y < 600;
}