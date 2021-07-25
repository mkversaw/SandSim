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

	bool empty;
	bool visited;

	pix() {
		col_r = 0;
		col_g = 0;
		col_b = 0;

		empty = true;
		visited = false;
	}

};

bool inBounds(int x, int y);
bool isFree(int x, int y);
void frameCounter(unsigned int& frame, Uint64& startTime);
void deletePixel(int x, int y);
void drawPixel(int x, int y);
void moveSimple(int x, int y);

const unsigned int width = 256;
const unsigned int height = 256;
bool left, right;

std::vector< unsigned char > pixels(width * height * 4, 0);
std::vector<std::vector<pix>> pixs(width, std::vector<pix>(height));

bool choice = false;

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* window = SDL_CreateWindow
	(
		"sandgame2",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, // initial window position
		512, 512,
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

	unsigned int frames = 0;
	Uint64 start = SDL_GetPerformanceCounter();

	while (running) {
	
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
				else if (event.button.button == SDL_BUTTON_RIGHT) { // clear the canvas
					for (int i = 0; i < width; i++) {
						for (int j = 0; j < height; j++) {
							pixs[i][j] = pix();
						}
					}
				}
			case SDL_MOUSEMOTION:
				if (leftMouseButtonDown) {
					unsigned char col_r = rand() % 256;
					unsigned char col_g = rand() % 256;
					unsigned char col_b = rand() % 256;

					m_x /= 2;
					m_y /= 2;

					for (int br_x = m_x - br_size; br_x < m_x + br_size; br_x++) {
						for (int br_y = m_y - br_size; br_y < m_y + br_size; br_y++) {
							if (inBounds(br_x, br_y) && pixs[br_x][br_y].empty) {
								pixs[br_x][br_y].empty = false;
								pixs[br_x][br_y].col_r = col_r;
								pixs[br_x][br_y].col_g = col_g;
								pixs[br_x][br_y].col_b = col_b;
							}
						}
					}

						
					
				}
				break;
			}

		}
		
		for (int i = width - 1; i > 0; i--) {
			choice = (rand() % 2) == 1;
			for (int j = height - 1; j > 0; j--) {
				if (!pixs[i][j].visited && !pixs[i][j].empty) { // not visited yet				
					moveSimple(i, j);
					continue;
				}

				drawPixel(i, j);
				
			}
		}

		//for (int i = 0; i < width; i++) {
		//	for (int j = 0; j < height; j++) {
		//		if (pixs[i][j].visited && pixs[i][j].empty) {
		//			pixs[i][j].empty = false;
		//			pixs[i][j].col_b = 0;
		//			pixs[i][j].col_g = 0;
		//			pixs[i][j].col_r = 255;
		//			unsigned int offset = (width * 4 * j) + i * 4;  // update the pixels to draw
		//			pixels[offset + 0] = pixs[i][j].col_b;          // b
		//			pixels[offset + 1] = pixs[i][j].col_g;          // g
		//			pixels[offset + 2] = pixs[i][j].col_r;          // r
		//		}
		//	}
		//}

		SDL_UpdateTexture
		(
			texture,
			NULL,
			pixels.data(),
			width * 4
		);
		
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		frameCounter(frames, start);

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

bool isFree(int x, int y) {
	if ( rand() % 25 == 1)
		return inBounds(x, y) && pixs[x][y].empty && !pixs[x][y].visited;
	return inBounds(x, y) && pixs[x][y].empty;
}

bool inBounds(int x, int y) {
	return x >= 0 && y >= 0 && x < width && y < height;
}

void frameCounter(unsigned int& frame, Uint64& startTime) {
	frame++;
	const Uint64 end = SDL_GetPerformanceCounter();
	const static Uint64 freq = SDL_GetPerformanceFrequency();
	const double seconds = (end - startTime) / static_cast<double>(freq);
	if (seconds > 2.0) {
		std::cout
			<< frame << " frames in "
			<< std::setprecision(1) << std::fixed << seconds << " seconds = "
			<< std::setprecision(1) << std::fixed << frame / seconds << " FPS ("
			<< std::setprecision(3) << std::fixed << (seconds * 1000.0) / frame << " ms/frame)"
			<< std::endl;
		startTime = end;
		frame = 0;
	}
}

void deletePixel(int x, int y) { // NOTE: does not actually delete the pixel
	pixs[x][y].empty = true;
	pixs[x][y].col_r = 0;
	pixs[x][y].col_b = 0;
	pixs[x][y].col_g = 0;
}

void drawPixel(int x, int y) {
	unsigned int offset = (width * 4 * y) + x * 4;  // update the pixels to draw
	pixels[offset + 0] = pixs[x][y].col_b;          // b
	pixels[offset + 1] = pixs[x][y].col_g;          // g
	pixels[offset + 2] = pixs[x][y].col_r;          // r
	pixs[x][y].visited = false;
}

void moveSimple(int x, int y) {
	pixs[x][y].visited = true;
	if (isFree(x, y + 1)) {
		pixs[x][y + 1] = pixs[x][y];
		deletePixel(x, y);
		return;
	}

	left = isFree(x - 1, y + 1);
	right = isFree(x + 1, y + 1);

	if (left && right) { // pick one
		choice ? right = false : left = false;
	}

	if (left) {
		pixs[x - 1][y + 1] = pixs[x][y];
		deletePixel(x, y);
	}
	else if (right) {
		pixs[x + 1][y + 1] = pixs[x][y];
		deletePixel(x, y);
	}
}