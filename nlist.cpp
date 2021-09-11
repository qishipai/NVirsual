// nlist.cpp 2021-08-04 by 云中龙++
#include <cmath>
#include "NVutils.h"
#include "NVmidi.h"
#include "NVsequ.h"
#include "NVnlist.h"
using namespace std;

template<typename T> struct rmPt     {typedef T t; };
template<typename T> struct rmPt<T*> {typedef T t; };

NVnote::NVnote(double T, const NVseq_event &E):
	Tstart(T), Tend(114514191981.0),
	track(E.track), channel(E.chan),
	key(E.num), vel(E.value)
{ }

NVnoteList::NVnoteList(const char *name):
	Tread(0.0), abstick(0), keys(nullptr)
{
	if ((error = !M.mid_open(name)))
	{
		return;
	}

	nv_print(name, "类型码: %4hu\n", M.type);
	nv_print(name, "轨道数: %4hu\n", M.tracks);
	nv_print(name, "分辨率: %4hu\n", M.ppnq);

	if ((error = M.type == 2))
	{
		nv_error("NVnlist", "%s: MIDI格式不支持！(type=2)\n");
		return;
	}

	S.seq_init(M); dT = 0.5 / M.ppnq;
	keys = new rmPt<decltype(keys)>::t [M.tracks];
}

NVnoteList::~NVnoteList()
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
				dT = 1e-6 * speed / M.ppnq;
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

void NVnoteList::OR()
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
