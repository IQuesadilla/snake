#include <SDL2/SDL.h>

time_t time(time_t *tloc);

int color_grid_box (SDL_Renderer *renderer, SDL_Point windowsize, SDL_Point gridsize, SDL_Point gridpoint, SDL_Color color)
{
	SDL_SetRenderDrawColor (renderer, color.r, color.g, color.b, color.a);

	SDL_Rect box;
	box.x = (gridpoint.x*((float)windowsize.x/gridsize.x))+1;
	box.y = (gridpoint.y*((float)windowsize.y/gridsize.y))+1;
	box.w = (windowsize.x/gridsize.x)-1;
	box.h = (windowsize.y/gridsize.y)-1;
	SDL_RenderFillRect (renderer, &box);
}

int draw_backdrop (SDL_Renderer *renderer, SDL_Point windowsize, SDL_Color windowcolor, SDL_Point gridsize, SDL_Color gridcolor)
{
	SDL_SetRenderDrawColor (renderer, windowcolor.r, windowcolor.g, windowcolor.b, windowcolor.a);
	SDL_RenderClear (renderer);

	SDL_SetRenderDrawColor (renderer, gridcolor.r, gridcolor.g, gridcolor.b, gridcolor.a);
	for (int w = 1; w < gridsize.x; w++)
		SDL_RenderDrawLine ( renderer, w*((float)windowsize.x/gridsize.x), 0, w*((float)windowsize.x/gridsize.x), windowsize.y);

	for (int h = 1; h < gridsize.y; h++)
		SDL_RenderDrawLine ( renderer, 0, h*((float)windowsize.y/gridsize.y), windowsize.x, h*((float)windowsize.y/gridsize.y));
}

int update_input (SDL_bool *enable_game_loop, int *direction)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_UP:
					case SDLK_DOWN:
					case SDLK_LEFT:
					case SDLK_RIGHT:
						*direction = event.key.keysym.sym;
					break;
				}
			break;

			case SDL_QUIT:
				*enable_game_loop = SDL_FALSE;
			break;
		}
	}
}

int update_food (SDL_Point *foodpoint, SDL_Point gridsize, SDL_Point playerpoint, SDL_Point **playertail, int *playerlength)
{
	if (playerpoint.x == foodpoint->x && playerpoint.y == foodpoint->y)
	{
		foodpoint->x = rand() % gridsize.x;
		foodpoint->y = rand() % gridsize.y;

		++(*playerlength);

		*playertail = realloc (*playertail,  (*playerlength+1) *(int)sizeof(SDL_Point));
		(*playertail)[*playerlength] = (SDL_Point){0,0};
	}
}

int update_player (SDL_bool *enable_game_loop, int playerdirection, SDL_Point gridsize, SDL_Point *playerpoint, SDL_Point *playertail, int playerlength)
{
	for (int l = playerlength; l > 0; --l)
	{
		playertail[l] = playertail[l-1];
	}
	if (playerlength > 0)
		playertail[0] = *playerpoint;

	switch (playerdirection)
	{
		case SDLK_UP:
				--playerpoint->y;
		break;

		case SDLK_DOWN:
				++playerpoint->y;
		break;

		case SDLK_LEFT:
				--playerpoint->x;
		break;

		case SDLK_RIGHT:
				++playerpoint->x;
		break;
	}

	if (playerpoint->y == -1 || playerpoint->y == gridsize.y || playerpoint->x == -1 || playerpoint->x == gridsize.x)
		*enable_game_loop = SDL_FALSE;

	for (int l = playerlength; l > 0; --l)
		if (playerpoint->x == playertail[l].x && playerpoint->y == playertail[l].y)
			*enable_game_loop = SDL_FALSE;
}

int main (int argc, char **agrv)
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *windowtexture = NULL;
	SDL_Point windowsize = {1000,1000};
	SDL_Color windowcolor = {0,0,0,255};
	SDL_Rect windowrect = {0, 0, windowsize.x, windowsize.y};
	SDL_Point gridsize = {15,15};
	SDL_Color gridcolor = {0,255,0,255};
	SDL_Point playerpoint = {0,0};
	SDL_Color playercolor = {255,0,0,255};
	int playerdirection = SDLK_RIGHT;
	SDL_Point *playertail = NULL;
	int playerlength = -1;
	SDL_Point foodpoint = playerpoint;
	SDL_Color foodcolor = {0,0,255,255};
	time_t seed; srand((unsigned) time(&seed));
	char windowtitle[32]; memset (windowtitle, '\0', 32);

	SDL_Init (SDL_INIT_VIDEO);

	window = SDL_CreateWindow ("snake",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			windowsize.x, windowsize.y,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED);
	windowtexture = SDL_CreateTexture (renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, windowsize.x, windowsize.y);

	SDL_bool enable_game_loop = SDL_TRUE;
	while (enable_game_loop == SDL_TRUE)
	{
		SDL_SetRenderTarget (renderer, windowtexture);
		update_input(&enable_game_loop, &playerdirection);

		draw_backdrop (renderer, windowsize, windowcolor, gridsize, gridcolor);

		update_food (&foodpoint, gridsize, playerpoint, &playertail, &playerlength);
		color_grid_box (renderer, windowsize, gridsize, foodpoint, foodcolor);

		update_player (&enable_game_loop, playerdirection, gridsize, &playerpoint, playertail, playerlength);
		color_grid_box (renderer, windowsize, gridsize, playerpoint, playercolor);

		for (int l = 0; l < playerlength; ++l)
			color_grid_box (renderer, windowsize, gridsize, playertail[l], playercolor);

		sprintf (windowtitle, "snake : %d", playerlength+1);
		SDL_SetWindowTitle (window, windowtitle);
		SDL_SetRenderTarget (renderer, NULL);
		SDL_RenderCopy (renderer, windowtexture, NULL, &windowrect);
		SDL_RenderPresent (renderer);
		SDL_Delay (150);
	}
	free (playertail);

	SDL_DestroyRenderer (renderer);
	SDL_DestroyWindow (window);

	SDL_Quit();
}
