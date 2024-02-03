#include "tetrisboard.h"
#ifndef io
#define io
#include <iostream>
#endif // !io
#include <SDL.h>
#include <tuple>
#include "config.h"
 
int block_size;
int BOARDX;
bool open = true;
int lastTime;
SDL_Rect bg;
SDL_Rect rect;
SDL_Event event;
SDL_Window* window;
SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
game g;


int ghosty;
bool updated = true;


//modify board with timed input
int leftdastimer=0;
int rightdastimer=0;
int softdroptimer = 0;
bool sd1 = 0;
static int timer=0;
bool left = 0;
bool right = 0;
bool softdrop = 0;
void update(game &g) { 
		timer = SDL_GetTicks();
		if (left && timer - leftdastimer > DAS)
		{
			g.move(1, -1);
			updated = true;
		}
		if (right && timer - rightdastimer > DAS)
		{
			g.move(1, 1);
			updated = true;
		}
	
	if (softdrop) 
	{
		if (timer - softdroptimer > DROPDAS)
		{
			g.y += g.softdropdist();
			updated = true;
		}
		else if(sd1) {
			sd1 = 0;
			g.sd();
			updated = true;
		}
	}
	
}

//get input
void input(game &g)
{
	
	while (SDL_PollEvent(&event))
	{
		if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
			open = false;
		}
		if (event.key.repeat == 0 && event.type == SDL_KEYDOWN)
		{
			int a = int(event.key.keysym.sym);
			if (a == HARDDROP)
			{
				updated = true;
				g.harddrop();
			}
			if (a == HOLD)
			{
				updated = true;
				g.hold();
			}
			if (a == SOFTDROP)
			{

				softdroptimer = SDL_GetTicks();
				sd1 = true;
				softdrop = true;
				updated = true;
			}
			if (a == CW)
			{
				g.rotate(1);
				updated = true;
			}
			if (a == CCW)
			{
				g.rotate(-1);
				updated = true;
			}
			if (a == RT)
			{
				g.rotate(2);
				updated = true;

			}
			if (a == LEFT)
			{
				g.move(0, -1);
				leftdastimer = SDL_GetTicks();
				left = 1;
				right = 0;
				updated = true;
			}
			if (a == RIGHT)
			{
				g.move(0, 1);
				rightdastimer = SDL_GetTicks();
				right = 1;
				left = 0;
				updated = true;
			}
			if (a == RESET)
			{
				g.reset();
				left = 0;
				right = 0;
				softdrop = 0;

				leftdastimer = 0;
				rightdastimer = 0;
				timer = 0;
				updated = true;
			}
			
		}
		if(event.type == SDL_KEYUP)
		{
			int a = int(event.key.keysym.sym);
			if (a == LEFT)
			{
				leftdastimer = 0;
				left = 0;
				
			}
			if (a == RIGHT)
			{
				rightdastimer = 0;
				right = 0;
			}
			if (a == SOFTDROP)
			{
				softdrop = false;
			}
		}
		
	}
}


int colors[9]{ //bg SZJLTOI garbage
	0x000000,
	0x59b101,
	0xd70f37,
	0x2141c6,
	0xe35b02,
	0xaf298a,
	0xe39f02,
	0x0f9bd7,
	0x666666
};
void color_from_rgb(uint32_t v) {
	SDL_SetRenderDrawColor(renderer, (v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF, 0xFF);
}
void rgba_from_rgb(uint32_t v) {
	SDL_SetRenderDrawColor(renderer, (v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF, 0x99);
}
void draw(game &g) {
	color_from_rgb(0x666666);
	SDL_RenderFillRect(renderer,&bg);
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 21; j++)
		{
			rect.x =1+ BOARDX+i * (block_size + 1);
			rect.y = -block_size/2+j * (block_size + 1);
			color_from_rgb(colors[g.board[j+9][i] + 1]);
			SDL_RenderFillRect(renderer, &rect);
			
		}
	}
	// queue
	for (int n = 0; n < 5; n++)
	{

		color_from_rgb(colors[g.queue[n]+1]);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				rect.x = 4*BOARDX + i * (block_size + 1);
				rect.y = block_size*3*n+j * (block_size + 1);
				if (g.piecedefs[g.queue[n]][0][j][i]!=-1)
				{
					SDL_RenderFillRect(renderer, &rect);
				}

			}
		}
	}
	//hold
	if (g.held_piece != -1)
	{
		color_from_rgb(colors[g.held_piece+1]);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (g.piecedefs[g.held_piece][0][j][i] != -1){
					
					rect.x = 0 + i * (block_size + 1);
					rect.y = j * (block_size + 1);
					SDL_RenderFillRect(renderer, &rect);
				}

			}
		}
	}
	//active
	if (active_piece)
	{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			rect.x = BOARDX + (g.x + i) * (block_size + 1);
			rect.y = block_size/2+(g.y - 10 + j) * (block_size + 1);
			if (g.piecedefs[g.active][g.rotation][j][i] != -1)
			{
				rgba_from_rgb(colors[g.piecedefs[g.active][g.rotation][j][i] + 1]);
				SDL_RenderFillRect(renderer, &rect);
			}

			}
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				rect.x = BOARDX + (3 + i) * (block_size + 1);
				rect.y = block_size / 2 + (-1 + j) * (block_size + 1);
				if (g.piecedefs[g.active][0][j][i] != -1)
				{
					rgba_from_rgb(colors[g.piecedefs[g.active][0][j][i] + 1]);
					SDL_RenderFillRect(renderer, &rect);
				}

		}
	}
	}

	//ghost
	if (ghost){
		ghosty= g.y+g.softdropdist();
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				rect.x = BOARDX + (g.x + i) * (block_size + 1);
				rect.y = block_size/2+(ghosty - 10 + j) * (block_size + 1);
				if (g.piecedefs[g.active][g.rotation][j][i] != -1)
				{
					rgba_from_rgb(colors[g.piecedefs[g.active][g.rotation][j][i] + 1]);
					SDL_RenderFillRect(renderer, &rect);
				}

			}
		}
	}
}


int frameCount, lastFrame, fps;
void render(game &g) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	frameCount++;
	int timerFPS = SDL_GetTicks() - lastFrame;
	if (timerFPS < (8)) {
		SDL_Delay((8) - timerFPS);
	}
	draw(g);

	SDL_RenderPresent(renderer);
}


void init(int pwh,int val) {
	if (val == NULL||pwh==NULL)
	{
		block_size = 40;
	}
	else {
		switch (pwh) //pixel width height
		{
		case 1:
			block_size = val;
			break;
		case 2:
			block_size = (val - 2) * 3 / 50 - 1;
		case 3:
			block_size = (val-32) / 20.5f - 1;
			break;
		default:
			break;
		}
	}
	int width = (block_size + 1) * 50 / 3 + 2;
	int height = 20.5f * (block_size + 1);
	
	BOARDX = width / 5;
	bg.x = BOARDX; bg.y = 0; bg.w = width * 3 / 5; bg.h = height;
	rect.x = block_size;rect.y = block_size;rect.w = block_size;rect.h = block_size;
	lastTime = 0;
	g.reset();
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
	SDL_SetWindowTitle(window, "Tetris");
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}


int main(int argv,char* argc[]) {
	init(t, size);
	while (open) {
		lastFrame = SDL_GetTicks();
		if (lastFrame >= (lastTime + 1000)) {
			lastTime = lastFrame;
			fps = frameCount;
			frameCount = 0;
			//std::cout<<"FPS" << fps << std::endl;
		}
		input(g);
		update(g);
		if (updated){
			if(g.cleared||g.spin)
				std::cout << g.cleared << " " << g.spin<<"\n";
			render(g);
			updated = false;
		}

	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
