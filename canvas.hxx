
#pragma once

extern const unsigned char KeyMap[128];

class Canvas
{
public:

    using color = std::complex<float>;

    struct RGBA_pix
    {
        unsigned char r, g, b, a;
    };

    SDL_Window   *Win;
    SDL_Renderer *Ren;

    int   TH, WinW, WinH;
    bool  KeyPress[128];
    color KeyColor[128];

    Canvas(); ~Canvas();

    void canvas_clear();

    void DrawKeyBoard();

    void Note(int k, int yb, int ye, color c);

private:

    SDL_Texture *Bk0, *Bk1, *Wk;
    SDL_Surface *colors;

    int BkeyW, BkeyH, WkeyW, WkeyH;
    int TW, TX[11], KeyX[128];

    int scale(int x) const;

    RGBA_pix getColor(color C) const;
};
