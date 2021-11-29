#include <cmath>
#include "Utils.hxx"
#include "MIDI.hxx"
#include "Sequ.hxx"
#include "Nlist.hxx"
using namespace std;
using namespace NVi;

NVnote::NVnote(double T, const NVseq_event &E):
    Tstart(T), Tend(1e20), track(E.track),
    chn(E.chan), key(E.num), vel(E.value){ }

bool NVnoteList::start_parse(const char *name)
{
    if (!M.mid_open(name))
    {
        return false;
    }

    if (M.type == 2)
    {
        error("Nlist", "%s: 类型不支持(fmt2)\n");
        info ("Nlist", "支持类型: fmt0 fmt1\n");
        return (M.mid_close(), false);
    }

    S.seq_start(M);
    abstick = 0; Tread = 0; dT = .5 / M.ppnq;
    keys = new stack<List_ptr> [M.tracks][128];
    return true;
}

void NVnoteList::destroy_all()
{
    delete[] keys; keys =  nullptr;
    M.mid_close(); S.seq_destroy();
}

void NVnoteList::list_seek(double T)
{
    if (T < Tread)
    {
        abstick = 0; Tread = 0; dT = .5 / M.ppnq;
        M.rewind_all(); S.seq_reset(M);
    }

    for (int k = 0; k < 128; L[k++].clear())
    {
        for (u16_t t = 0; t < M.tracks; ++t)
        {
            while (!keys[t][k].empty())
            {
                keys[t][k].pop();
            }
        }
    }

    while (S.event().track < M.tracks)
    {
        const NVseq_event  &E  =  S.event();
        Tread += dT * (E.abstick - abstick);
        abstick = E.abstick;

        if (Tread >= T){ break; }

        if (E.type == NV_METYPE::META && E.num == 0x51u)
        {
            u32_t   speed    =   E.data[0];
            speed = speed << 8 | E.data[1];
            speed = speed << 8 | E.data[2];
            dT  = .000001 * speed / M.ppnq;
        }

        S.seq_next(M);
    }
}

void NVnoteList::update_to(double T)
{
    while (S.event().track < M.tracks)
    {
        const NVseq_event  &E  =  S.event();
        Tread += dT * (E.abstick - abstick);
        abstick = E.abstick;

        if (Tread >= T){ break; }

        switch (E.type)
        {
        case (NV_METYPE::NOON):

            if (E.value > 0)
            {
                L[E.num].emplace_back(Tread, E);
                List_ptr nt = L[E.num].end();
                keys[E.track][E.num].push(--nt);
                break;
            }

            goto _NOFF;

        case (NV_METYPE::META):

            if (E.num == 0x51u)
            {
                u32_t   speed    =   E.data[0];
                speed = speed << 8 | E.data[1];
                speed = speed << 8 | E.data[2];
                dT  = .000001 * speed / M.ppnq;
            }

            break;

        case (NV_METYPE::NOFF): _NOFF:

            if (!keys[E.track][E.num].empty())
            {
                keys[E.track][E.num].top()->Tend = Tread;
                keys[E.track][E.num].pop();
            }

        default: break;
        }

        S.seq_next(M);
    }
}

void NVnoteList::VisualFit(double T)
{
    for (u16_t k = 0; k < 128; ++k)
    {
        double T0 = 1e20, T1 = 1e20;
        List_ptr p = L[k].end();

        while (p-- != L[k].begin())
        {
            if (T0 < p->Tend && p->Tend < T1)
            {
                p->Tend = T0, T0 = p->Tstart;

                if (p->Tend - p->Tstart < T)
                {
                    double t = (p++)->Tstart;
                    (p--)->Tstart = t;
                    p = L[k].erase(p);
                }
            }
            else if (T0 - p->Tstart < T * 2)
            {
                u16_t tr = (++p)->track;

                if (tr == (--p)->track)
                {
                    double t = (p++)->Tstart;
                    (p--)->Tstart = t;
                    p = L[k].erase(p);
                }
            }
            else
            {
                T0 = p->Tstart, T1 = p->Tend;
            }
        }
    }
}

void NVnoteList::remove_to(double T)
{
    for (u16_t k = 0; k < 128; ++k)
    {
        List_ptr p = L[k].begin();

        while (p != L[k].end() && p->Tstart < T)
        {
            p->Tend < T? (p = L[k].erase(p)) : ++p;
        }
    }
}
