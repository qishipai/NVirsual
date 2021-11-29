#include <cstdio>
#include <complex>
#include <SDL2/SDL.h>
#include <bassmidi.h>
#include "Utils.hxx"
#include "MIDI.hxx"
#include "Sequ.hxx"
#include "Nlist.hxx"
#include "canvas.hxx"
using namespace NVi;
using namespace std;

NVnoteList MIDI;
Canvas     *Win;
Canvas::color *Col;

SDL_Event Evt;
HSTREAM   Stm;

SDL_Surface *Scr;

int    _WinH, pps = 2000;
double Tplay = 0.0, Tscr;

static void DrawNote(u16_t k, const NVnote &n)
{
    Canvas::color c = Col[n.track];
    int key = KeyMap[k], y_1;
    int y_0 = _WinH - (n.Tstart - Tplay) * pps;

    if (y_0 < 0)
    {
        y_0 = 0;
    }
    else
    {
        if (y_0 > _WinH)
        {
            y_0 = _WinH;
        }
    }

    if (n.Tend < Tplay + Tscr)
    {
        y_1 = _WinH - (n.Tend - Tplay) * pps;

        if (y_1 < 0)
        {
            y_1 = 0;
        }
        else
        {
            if (y_1 > _WinH)
            {
                y_1 = _WinH;
            }
        }
    }
    else
    {
        y_1 = 0;
    }

    if (n.Tstart <= Tplay && Tplay < n.Tend)
    {
        Win->KeyPress[key] = true;
        Win->KeyColor[key] = (c *= 0.8);
    }

    Win->Note(k, y_0, y_1, c);
}

BOOL CALLBACK filter(HSTREAM S, DWORD trk, BASS_MIDI_EVENT *E, BOOL sk, void *u)
{
    if (E->event == MIDI_EVENT_NOTE)
    {
        int vel = HIBYTE(E->param);
        return vel == 0 || vel > 9;
    }

    return TRUE;
}

#if defined(_WIN32) || defined(_WIN64)
#define BASSMIDI_LIB "bassmidi.dll"
#else
#define BASSMIDI_LIB "libbassmidi.so"
#endif

int NVmain(int ac, char **av)
{
    if (ac != 3)
    {
        info("NVplayer", "Usage> %s <*.mid> <*.sf2/*.sfz>\n", av[0]);
        return 1;
    }

    if (!MIDI.start_parse(av[1]))
    {
        Canvas C; SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
            "Error!!!!!", "MIDI File Load Failed", nullptr);
        return 1;
    }

    Win = new Canvas; Col = new Canvas::color [MIDI.M.tracks];

    for (int i = 0; i < MIDI.M.tracks; ++i)
    {
        float ang = 6.283185307 * i / (MIDI.M.tracks + 1) * 4;
        Col[i] = complex<float>(cosf(ang), sinf(ang)) * 512.0f;
    }

    _WinH = Win->WinH - Win->TH; Tscr = (double)_WinH / pps;

    BASS_PluginLoad(BASSMIDI_LIB, 0);
    BASS_SetConfig(BASS_CONFIG_MIDI_AUTOFONT, 0);
    BASS_Init(-1, 44100, 0, 0, nullptr);

    Stm = BASS_StreamCreateFile(0, av[1], 0, 0, 0);

    HSOUNDFONT Sf = BASS_MIDI_FontInit(av[2], 0);
    BASS_MIDI_FONT FontSet{Sf, -1, 0};
    BASS_MIDI_FontSetVolume(Sf, 0.15);
    BASS_MIDI_StreamSetFonts(Stm, &FontSet, 1);

    HFX Fx1 = BASS_ChannelSetFX(Stm, BASS_FX_DX8_REVERB, 1);
    BASS_DX8_REVERB Reverb{0, -5, 500, 0.01};
    BASS_FXSetParameters(Fx1, &Reverb);

    BASS_ChannelSetAttribute(Stm, BASS_ATTRIB_MIDI_VOICES, 800);

    BASS_MIDI_StreamSetFilter(Stm, 0, filter, nullptr);
    BASS_ChannelPlay(Stm, 1);

    while (BASS_ChannelIsActive(Stm) != BASS_ACTIVE_STOPPED)
    {
        MIDI.update_to(Tplay + Tscr);
        MIDI.remove_to(Tplay); MIDI.VisualFit(Tscr * .005);
        Win->canvas_clear();

        for (int k = 0; k < 128; ++k)
        {
            for (const NVnote &n : MIDI.L[KeyMap[k]])
            {
                DrawNote(k, n);
            }
        }

        Win->DrawKeyBoard(); SDL_RenderPresent(Win->Ren);

        Tplay = BASS_ChannelBytes2Seconds(Stm, BASS_ChannelGetPosition(Stm, BASS_POS_BYTE));

        while (SDL_PollEvent(&Evt))
        {
            switch (Evt.type)
            {
            case (SDL_FINGERDOWN):

                BASS_ChannelSetPosition(Stm, BASS_ChannelSeconds2Bytes(Stm, Tplay + (Evt.tfinger.x - 0.5) * 10), BASS_POS_BYTE);
                Tplay = BASS_ChannelBytes2Seconds(Stm, BASS_ChannelGetPosition(Stm, BASS_POS_BYTE));
                MIDI.list_seek(Tplay);
                break;

            default: break;
            }
        }
    }

    delete Win; delete[] Col;
    BASS_Free(); BASS_PluginFree(0); MIDI.destroy_all();
    return 0;
}

int main(int ac, char **av)
{
    char midi[] = "/sdcard/default.mid", sf[] = "/sdcard/default.sf2";
    char *arg[4]{nullptr, midi, sf, nullptr};
    return NVmain(3, arg);
}
