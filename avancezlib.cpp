#include "avancezlib.h"

// Creates the main window. Returns true on success.
bool AvancezLib::init(int width, int height)
{
	SDL_Log("Initializing the system...\n");

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL failed the initialization: %s\n", SDL_GetError());
		return false;
	}

	//Create window
	window = SDL_CreateWindow("aVANCEZ", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	//Create renderer for window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	TTF_Init();
	font = TTF_OpenFont("Resources/qix.ttf", 32); //this opens a font style and sets a size
	if (font == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "font cannot be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	// initialize the keys
	key.slow = false;	key.left = false;	key.right = false; key.up = false; key.down = false;

	//Initialize renderer color
	SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);

	//Clear screen
	SDL_RenderClear(renderer);

	SDL_Log("System up and running...\n");
	return true;
}


// Destroys the avancez library instance
void AvancezLib::destroy()
{
	SDL_Log("Shutting down the system\n");

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_CloseFont(font);

	TTF_Quit();
	SDL_Quit();
}


bool AvancezLib::update()
{
	bool go_on = true;
	SDL_Event event;


	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			go_on = false;

		if (event.type == SDL_KEYDOWN)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			case SDLK_q:
				go_on = false;
				break;
			case SDLK_SPACE:
				key.slow = true;
				break;
			case SDLK_LEFT:
				key.left = true;
				break;
			case SDLK_RIGHT:
				key.right = true;
				break;
            case SDLK_UP:
                key.up = true;
                break;
            case SDLK_DOWN:
                key.down = true;
                break;
			}
		}

		if (event.type == SDL_KEYUP)
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_SPACE:
				key.slow = false;
				break;
			case SDLK_LEFT:
				key.left = false;
				break;
			case SDLK_RIGHT:
				key.right = false;
				break;
            case SDLK_UP:
                key.up = false;
                break;
            case SDLK_DOWN:
                key.down = false;
                break;
			}
		}

	}

	//Update screen
	SDL_RenderPresent(renderer);

	//Clear screen
	SDL_RenderClear(renderer);

	return go_on;
}


Sprite * AvancezLib::createSprite(const char * path)
{
	SDL_Surface* surf = IMG_Load(path);
	if (surf == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to load image %s! SDL_image Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Create texture from surface pixels
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		return NULL;
	}

	//Get rid of old loaded surface
	SDL_FreeSurface(surf);

	Sprite * sprite = new Sprite(renderer, texture);

	return sprite;
}

Sprite * AvancezLib::createSpriteByColor(int r, int g, int b, int a)
{

    //Create texture from color
    SDL_Surface *surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0);
    if (surface == NULL)
    {
        SDL_Log("Create Surf error!");
        return NULL;
    }

    Uint32 color = SDL_MapRGB(surface->format,r, g ,b);
    SDL_FillRect(surface, 0, color);
    
    //Create texture from surface pixels
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);
   
    //Get rid of old loaded surface
    SDL_FreeSurface(surface);
    
    Sprite * sprite = new Sprite(renderer, texture);
    
    return sprite;
}

void AvancezLib::drawText(int x, int y, const char * msg, float size)
{
	SDL_Color black = { 0, 0, 0 };  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
    SDL_Color white = { 255, 255, 255 };

    
	SDL_Surface* surf = TTF_RenderText_Solid(font, msg, white); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

	SDL_Texture* msg_texture = SDL_CreateTextureFromSurface(renderer, surf); //now you can convert it into a texture

	int w = 0;
	int h = 0;
	SDL_QueryTexture(msg_texture, NULL, NULL, &w, &h);
	SDL_Rect dst_rect = { x, y, (int)(w * size), (int)(h*size) };

	SDL_RenderCopy(renderer, msg_texture, NULL, &dst_rect);

	SDL_DestroyTexture(msg_texture);
	SDL_FreeSurface(surf);
}

float AvancezLib::getElapsedTime()
{
	return SDL_GetTicks() / 1000.f;
}

void AvancezLib::getKeyStatus(KeyStatus & keys)
{
	keys.slow = key.slow;
	keys.left = key.left;
	keys.right = key.right;
    keys.up = key.up;
    keys.down = key.down;
}


Sprite::Sprite(SDL_Renderer * renderer, SDL_Texture * texture)
{
	this->renderer = renderer;
	this->texture = texture;
}


void Sprite::draw(int x, int y, float angle,float width, float height)
{
	SDL_Rect rect;

	rect.x = x;
	rect.y = y;

    if(width == -1 && height == -1)
        SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));
    else{
        rect.w = width;
        rect.h = height;
    }
    
	SDL_Point center;
	center.x = (int)(rect.w / 2.f);
	center.y = (int)(rect.h / 2.f);

	//Render texture to screen
//	SDL_RenderCopy(renderer, texture, NULL, &rect);

	SDL_RenderCopyEx(renderer,
		texture,
		NULL,
		&rect,
		angle,
		&center,
		SDL_FLIP_NONE);
}


void Sprite::destroy()
{
	SDL_DestroyTexture(texture);
}

