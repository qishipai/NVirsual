#include <cstdio>
#include <complex>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <bassmidi.h>
#include "NVutils.h"
#include "NVmidi.h"
#include "NVsequ.h"
#include "NVnlist.h"
#include "canvas.h"
using namespace std;

SDL_Event Evt;
HSTREAM   Stm;
Canvas::color *Col;

Canvas *Win;
SDL_Surface *Scr;

int _WinH, pps = 1000;
double Tplay = 0.0, Tscr;

void DrawNote(u16_t k, const NVnote &n)
{
	complex<float> c = Col[n.track];

	int key = KeyMap[k], y_0 = _WinH - (n.Tstart - Tplay) * pps;
	int y_1 = n.Tend < Tplay + Tscr? _WinH - (n.Tend - Tplay) * pps : 0;

	if (y_0 < 0) {y_0 = 0; }    if (y_0 > _WinH) {y_0 = _WinH; }
	if (y_1 < 0) {y_1 = 0; }    if (y_1 > _WinH) {y_1 = _WinH; }

	if (n.Tstart <= Tplay && Tplay < n.Tend)
	{
		Win->KeyColor[key] = (c *= 0.8); Win->KeyPress[key] = true;
	}

	Win->Note(k, y_0, y_1, c);
}

BOOL CALLBACK filter(HSTREAM S, DWORD trk, BASS_MIDI_EVENT *E, BOOL sk, void *u)
{
	if (E->event == MIDI_EVENT_NOTE)
	{
		int vel = HIBYTE(E->param);
		return vel == 0 || vel > 5;
	}

	return TRUE;
}

int main(int ac, char **av)
{
	if (ac != 3)
	{
		nv_print("NVplayer", "Usage> %s <*.mid> <*.sf2/*.sfz>\n", av[0]);
		return 1;
	}

	NVnoteList NL(av[1]); if (NL.error) { return 1; }

	Win = new Canvas;

	SDL_Texture *img[8];
	SDL_Rect img_box{0, 0, 179, 213};
	int _c = 0;

	Col = new Canvas::color [NL.M.tracks];

	for (int i = 0; i < NL.M.tracks; ++i)
	{
		float ang  = 6.283185307 * i / NL.M.tracks * 4;
		complex<float> A(cosf(ang), sinf(ang));
		Col[i] = A * complex<float>(powf(1.01, i / 4) * 512.0f);
	}

	for (int i = 0; i < 8; ++i)
	{
		char pth[128];
		snprintf(pth, 128, "%d.jpg", i);
		SDL_Surface *Sur = IMG_Load(pth);
		img[i] = SDL_CreateTextureFromSurface(Win->Ren, Sur);
		SDL_FreeSurface(Sur);
	}

	_WinH = Win->WinH - Win->TH; Tscr = (double)_WinH / pps;

	BASS_PluginLoad("libbassmidi.so", 0);
	BASS_SetConfig(BASS_CONFIG_MIDI_AUTOFONT, 0);
	BASS_Init(-1, 44100, 0, 0, nullptr);

	Stm = BASS_StreamCreateFile(0, av[1], 0, 0, 0);

	BASS_ChannelSetAttribute(Stm, BASS_ATTRIB_MIDI_VOICES, 800);

	HFX Fx1 = BASS_ChannelSetFX(Stm, BASS_FX_DX8_REVERB, 1);
	BASS_DX8_REVERB Reverb{0, -5, 500, 0.0001};
	BASS_FXSetParameters(Fx1, &Reverb);

	BASS_MIDI_FONT FontSet{BASS_MIDI_FontInit(av[2], 0), -1, 0};
	BASS_MIDI_FontSetVolume(FontSet.font, 0.2);
	BASS_MIDI_StreamSetFonts(Stm, &FontSet, 1);

	BASS_MIDI_StreamSetFilter(Stm, 0, filter, nullptr);

	BASS_ChannelPlay(Stm, 1);

	while (BASS_ChannelIsActive(Stm) != BASS_ACTIVE_STOPPED)
	{
		NL.update_to(Tplay + Tscr);
		NL.remove_to(Tplay); NL.OR();

		Win->canvas_clear();

		int c = 0;

		for (int k = 0; k < 128; ++k)
		{
			for (const NVnote &n : NL.L[KeyMap[k]])
			{
				DrawNote(k, n); ++c;
			}
		}

		int st = (int)(c > 500) + (c > 1000) + (c > 2000)
		 + (c > 5000) + (c > 10000) + (c > 19000) + (c > 25000);

		if (st > 5)
		{
			st = c > _c - 100? 6 : 7;
		}

		_c = c; Win->DrawKeyBoard();
		SDL_RenderCopy(Win->Ren, img[st], nullptr, &img_box);
		while (SDL_PollEvent(&Evt)); SDL_RenderPresent(Win->Ren);
		Tplay = BASS_ChannelBytes2Seconds(Stm, BASS_ChannelGetPosition(Stm, BASS_POS_BYTE));
	}

	for (int i = 0; i < 8; ++i)
	{
		SDL_DestroyTexture(img[i]);
	}

	delete[] Col; delete Win; BASS_Free(); BASS_PluginFree(0);
	return 0;
}
