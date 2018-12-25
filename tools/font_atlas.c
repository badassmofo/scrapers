#include <SDL/SDL.h>
#undef main
#include <SDL/SDL_ttf.h>
#include <stdbool.h>

int main(int argc, const char *argv[]) {
	if (argc <= 1) {
		printf("ERROR! NO FONT PASSED IN ARGUMENT!\n");
		return EXIT_FAILURE;
	}

	int32_t font_size    = 28;
	bool show_window     = false;
	const char* font_loc = NULL;

	/* Parse the arguments */
	for (size_t i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			if (strlen(argv[i]) > 2)
				continue;

			switch (argv[i][1]) {
				case 'f':
				case 'F': /* Specify font file */
					if (i + 1 >= argc) {
						printf("ERROR! INVALID ARGUMENTS!\n");
						return EXIT_FAILURE;
					}
					font_loc = argv[i + 1];
					break;
				case 's':
				case 'S': /* Specify font size */
					if (i + 1 >= argc) {
						printf("ERROR! INVALID ARGUMENTS!\n");
						return EXIT_FAILURE;
					}

					font_size = atoi(argv[i + 1]);
					if (font_size <= 0) {
						printf("ERROR! INVALID FONT SIZE \"%d\"!\n", font_size);
						return EXIT_FAILURE;
					}
					break;
				case 'w':
				case 'W':
					show_window = true;
					break;
			}
		}
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		printf("ERROR! FAILED TO INIT SDL!\nMESSAGE: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	if (TTF_Init() == -1) {
		printf("ERROR! FAILED TO INIT SDL_TTF!\nMESSAGE: %s\n", TTF_GetError());
		return EXIT_FAILURE;
	}

	SDL_Surface* screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE);
	if (!screen) {
		printf("ERROR! FAILED TO CREATE SCREEN!\nMESSAGE: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	SDL_WM_SetCaption("BITMAP FONT GEN", NULL);

	TTF_Font* font = TTF_OpenFont(font_loc, font_size);
	if (!font) {
		printf("ERROR! FAILED TO LOAD FONT!\nMESSAGE: %s\n", TTF_GetError());
		return EXIT_FAILURE;
	}
	
	/* Create large 2000x2000 black surface to hold all characters */
	SDL_Color fore_col = { 255, 255, 255, 255 };
	SDL_Surface* ret = SDL_CreateRGBSurface(SDL_HWSURFACE, 2000, 2000, 32, 0, 0, 0, 0);
	SDL_FillRect(ret, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

	FILE* output_file = fopen("output.txt", "w");
	if (!output_file) {
		printf("ERROR! FAILED TO CREATE TEXT FILE!\n");
		return EXIT_FAILURE;
	}

	SDL_Rect offset = { 0, 0, 0, 0 };
	SDL_Rect final_size = { 0, 0, 0, 0 };
	uint32_t largest_char = 0;
	for (size_t ii = 32; ii < 127; ++ii) { /* Loop from space to tilde */
		char* c = (char*)malloc(sizeof(char));
		c[0] = (char)ii;
		c[1] = '\0';

		/* Render character and blit it to master surface */
		SDL_Surface* c_ttf = TTF_RenderText_Solid(font, c, fore_col);
		if (c_ttf->h > largest_char) largest_char = c_ttf->h; /* Check if this character is largest on the line, used to place next row */
		SDL_BlitSurface(c_ttf, NULL, ret, &offset);
		fprintf(output_file, "%d %d %d %d %d\n", ii, offset.x, offset.y, c_ttf->w, c_ttf->h);

		offset.x += c_ttf->w + 1; /* Add width to offset */
		if (offset.x > 640) { /* Getting a little long, make a new row. Pointless, really, adds more work if anything. */
			if (offset.x > final_size.w) final_size.w = offset.x; /* Check if this row is longest, used to determine final size of master surface */
			offset.x  = 0;
			offset.y += largest_char + 1;
			largest_char = 0;
		}

		SDL_FreeSurface(c_ttf);
		free(c);
	}
	fclose(output_file);

	/* Create the final render surface */
	final_size.h = offset.y + largest_char;
	SDL_Surface* final = SDL_CreateRGBSurface(SDL_HWSURFACE, final_size.w, final_size.h, 32, 0, 0, 0, 0);
	SDL_BlitSurface(ret, NULL, final, NULL);
	SDL_FreeSurface(ret);

	/* If -w/W paramter is passed, preview the final product in a window before saving it */
	if (show_window) {
		SDL_Event event;
		bool running = true;
		while (running) {
			if (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_QUIT:
						running = false;
						break;
					case SDL_KEYDOWN:
						switch (event.key.keysym.sym) {
							case SDLK_ESCAPE:
							case SDLK_q:
								running = false;
								break;
							default: break;
						}
						break;
					default: break;
				}
			}

			SDL_BlitSurface(final, NULL, screen, NULL);
			if (SDL_Flip(screen) == -1) {
				printf("ERROR! FAILED TO FLIP SCREEN!\nMESSAGE: %s\n", SDL_GetError());
				return EXIT_FAILURE;
			}
		}
	}

	/* Save BMP and clean up */
	SDL_SaveBMP(final, "output.bmp");
	SDL_FreeSurface(final);
	TTF_CloseFont(font);

	TTF_Quit();
	SDL_Quit();
	return EXIT_SUCCESS;
}

