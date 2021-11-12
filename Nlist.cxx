#include <cmath>
#include "Utils.hxx"
#include "MIDI.hxx"
#include "Sequ.hxx"
#include "Nlist.hxx"
using namespace NV;
using namespace std;

NVnote::NVnote(double T, const NVseq_event &E):
    Tstart(T), Tend(114514191981.0), track(E.track),
    channel(E.chan), key(E.num), vel(E.value) { }

bool NVnoteList::start_parse(const char *name)
{
    if (!M.mid_open(name))
    {
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)

    print("INFO", "类型码: %4hu\n", M.type  );
    print("INFO", "轨道数: %4hu\n", M.tracks);
    print("INFO", "分辨率: %4hu\n", M.ppnq  );

#else

    print("INFO", "类型码: \e[35m%4hu\e[m\n", M.type  );
    print("INFO", "轨道数: \e[33m%4hu\e[m\n", M.tracks);
    print("INFO", "分辨率: \e[36m%4hu\e[m\n", M.ppnq  );

#endif

    Tread = 0.0; abstick = 0;

    if (M.type == 2)
    {
        M.mid_close();
        error("Nlist", "%s: 类型不支持！(type=2)\n");
        return false;
    }

    S.seq_init(M); dT = 0.5 / M.ppnq;
    keys = new rmPR<decltype(keys)>::t [M.tracks];
    return true;
}

void NVnoteList::destroy_all()
{
    delete[] keys; keys =  nullptr;
    M.mid_close(); S.seq_destroy();
}

void NVnoteList::update_to(double T)
{
    while (S.event().track < M.tracks && Tread < T)
    {
        const NVseq_event &E = S.event();

        if (E.abstick != abstick)
        {
            Tread += dT * (E.abstick - abstick);
            abstick = E.abstick;
        }

        switch (E.type)
        {
        case (NV_METYPE::OTHER):

            if (E.num == 0x51u)
            {
                u32_t speed = *E.data;
                (speed <<= 8) |= *(E.data + 1);
                (speed <<= 8) |= *(E.data + 2);
                dT = 0.000001 * speed / M.ppnq;
            }

            break;

        case (NV_METYPE::NOTEON):

            if (E.value > 0)
            {
                L[E.num].emplace_back(Tread, E);
                auto p = L[E.num].end();
                keys[E.track][E.num].push(--p);
                break;
            }

        case (NV_METYPE::NOTEOFF):

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

void NVnoteList::OR()             // 大概有用吧
{
    for (int i = 0; i < 128; ++i)
    {
        list<NVnote>::iterator p = L[i].end();
        double T0 = 114514191981.0;
        double T1 = 114514191981.0;

        while (p-- != L[i].begin())
        {
            if (T0 < p->Tend && p->Tend < T1)
            {
                p->Tend = T0, T0 = p->Tstart;

                if (p->Tend - p->Tstart < 1e-7)
                {
                    p = L[i].erase(p);
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
    for (int i = 0; i < 128; ++i)
    {
        list<NVnote>::iterator p = L[i].begin();

        while (p != L[i].end() && p->Tstart < T)
        {
            p->Tend < T? (p = L[i].erase(p)) : ++p;
        }
    }
}
