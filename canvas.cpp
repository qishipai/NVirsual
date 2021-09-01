#include <complex>
#include <cmath>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "canvas.h"
using namespace std;

const unsigned char KeyMap[128] =
{
	0,   2,   4,   5,   7,   9,   11,  12,  14,  16,  17,  19,  21,  23,  24,  26,
	28,  29,  31,  33,  35,  36,  38,  40,  41,  43,  45,  47,  48,  50,  52,  53,
	55,  57,  59,  60,  62,  64,  65,  67,  69,  71,  72,  74,  76,  77,  79,  81,
	83,  84,  86,  88,  89,  91,  93,  95,  96,  98,  100, 101, 103, 105, 107, 108,
	110, 112, 113, 115, 117, 119, 120, 122, 124, 125, 127, 1,   3,   6,   8,   10,
	13,  15,  18,  20,  22,  25,  27,  30,  32,  34,  37,  39,  42,  44,  46,  49,
	51,  54,  56,  58,  61,  63,  66,  68,  70,  73,  75,  78,  80,  82,  85,  87,
	90,  92,  94,  97,  99,  102, 104, 106, 109, 111, 114, 116, 118, 121, 123, 126, 
};

static const int genKeyX[12] =
{
	3, 58, 101, 172, 199, 297, 346, 395, 458, 492, 570, 590,
};

Canvas::Canvas()
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	Win = SDL_CreateWindow("NVplayer++", 0, 0, 1280, 720, 0);
	Ren = SDL_CreateRenderer(Win, -1, SDL_RENDERER_SOFTWARE);

	colors = IMG_Load("ColorTable.png");

	SDL_Surface *Sur0 = IMG_Load("Bk_0.png");
	SDL_Surface *Sur1 = IMG_Load("Bk_1.png");
	SDL_Surface *Sur2 = IMG_Load("Wk_0.png");
	Bk0 = SDL_CreateTextureFromSurface(Ren, Sur0);
	Bk1 = SDL_CreateTextureFromSurface(Ren, Sur1);
	Wk  = SDL_CreateTextureFromSurface(Ren, Sur2);
	SDL_FreeSurface(Sur0);
	SDL_FreeSurface(Sur1); SDL_FreeSurface(Sur2);

	SDL_GetWindowSize(Win, &WinW, &WinH);

	for (int i = 0; i < 11; ++i)
	{
		TX[i] = scale(i * 685);
	}

	for (int i = 0; i < 128; ++i)
	{
		KeyX[i] = scale(i / 12 * 685 + genKeyX[i % 12]);
	}

	TW    = scale(685), TH    = scale(610);
	BkeyW = scale(58 ), WkeyW = scale(93 );
	BkeyH = scale(390), WkeyH = scale(608);
}

Canvas::~Canvas()
{
	SDL_FreeSurface(colors); SDL_DestroyTexture(Bk0);
	SDL_DestroyTexture(Bk1); SDL_DestroyTexture(Wk);
	SDL_DestroyRenderer(Ren); SDL_DestroyWindow(Win);
	IMG_Quit(); SDL_Quit();
}

void Canvas::canvas_clear()
{
	SDL_SetRenderDrawColor(Ren, 50, 50, 50, 255);
	SDL_RenderClear(Ren);

	for (int i = 0; i < 128; ++i)
	{
		KeyColor[i] = 0.0f, KeyPress[i] = false;
	}
}

void Canvas::DrawKeyBoard()
{
	static color d(cosf(0.01), -sinf(0.01));
	int y = WinH - TH + 1, i, y_;

	for (i = 0, y_ = y + WkeyH; i < 75; ++i)
	{
		int k = KeyMap[i], x_ = KeyX[k] + WkeyW;
		color c = KeyColor[k];

		for (int _y = y; _y < y_; ++_y, c *= d)
		{
			const RGBA_pix &p = getColor(c);
			SDL_SetRenderDrawColor(Ren, p.r, p.g, p.b, p.a);
			SDL_RenderDrawLine(Ren, KeyX[k], _y, x_, _y);
		}
	}

	for (i = 0; i < 11; ++i)
	{
		SDL_Rect Box{TX[i], y - 1, TW, TH};
		SDL_RenderCopy(Ren, Wk, nullptr, &Box);
	}

	for (i = 75, y_ = y + BkeyH; i < 128; ++i)
	{
		int k = KeyMap[i], x_ = KeyX[k] + BkeyW;
		color c = KeyColor[k];

		for (int _y = y; _y < y_; ++_y, c *= d)
		{
			const RGBA_pix &p = getColor(c);
			SDL_SetRenderDrawColor(Ren, p.r, p.g, p.b, p.a);
			SDL_RenderDrawLine(Ren, KeyX[k], _y, x_, _y);
		}
	}

	for (i = 75; i < 128; ++i)
	{
		int k = KeyMap[i];
		SDL_Rect Box{KeyX[k], y - 1, BkeyW + 2, BkeyH + 3};
		SDL_RenderCopy(Ren, KeyPress[k]? Bk1 : Bk0, nullptr, &Box);
	}
}

void Canvas::Note(int k, int yb, int ye, color c)
{
	int x  = KeyX[KeyMap[k]], w = k >= 75? BkeyW : WkeyW;
	int x_ = x + w, h = yb - ye + 1;

	for (int _x = x; _x < x_; ++_x, c *= 1.02)
	{
		const RGBA_pix &p = getColor(c);
		SDL_SetRenderDrawColor(Ren, p.r, p.g, p.b, p.a);
		SDL_RenderDrawLine(Ren, _x, yb, _x, ye);
	}

	if (yb - ye > 4)
	{
		SDL_SetRenderDrawColor(Ren, 0, 0, 0, 255);
		SDL_Rect R{x, ye, w + 1, h}; SDL_RenderDrawRect(Ren, &R);
	}
}

int Canvas::scale(int x) const
{
	return (x * WinW + 3661) / 7322;
}

Canvas::RGBA_pix Canvas::getColor(color C) const
{
	int x = C.real() + 1024.5f, y = C.imag() + 1024.5f;
	return *((RGBA_pix*)colors->pixels + (x + y * 2048));
}
