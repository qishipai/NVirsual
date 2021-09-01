// sequ.cpp 2021-08-31 by 云中龙++
#include "NVutils.h"
#include "NVmidi.h"
#include "NVsequ.h"

void NVsequencer::seq_init(NVmidiFile &midi)
{
	E = new NVseq_event [1 + midi.tracks];
	E[midi.tracks].abstick = 0xFFFFFFFFu;
	E[midi.tracks].track   = midi.tracks;

	for (len = 1; len < midi.tracks; len <<= 1);

	T = new u16_t [len << 1];

	for (int i = midi.tracks; i < len; ++i)
	{
		T[i + len] = midi.tracks;
	}

	for (u16_t i = 0; i < midi.tracks; ++i)
	{
		if (E[i].get(i, midi))
		{
			E[i].track   = i;
			T[len + i]   = i;
			E[i].abstick = E[i].tick;
		}
		else
		{
			T[len + i] = midi.tracks;
		}
	}

	for (int i = len - 1; i > 0; --i)
	{
		update(i, T[i << 1], T[i << 1 | 1]);
	}
}

const NVseq_event& NVsequencer::event()
{
	return E[T[1]];
}

void NVsequencer::seq_next(NVmidiFile &midi)
{
	int a = T[1], p = len + a;

	if (midi.trk_over[a] || !E[a].get(a, midi))
	{
		T[p] = midi.tracks;
	}
	else
	{
		E[a].abstick += E[a].tick;
	}

	for ( ; (a = p >> 1); p = a)
	{
		update(a, T[p], T[p ^ 1]);
	}
}

void NVsequencer::seq_destroy()
{
	delete[]  T; delete[]  E;
	T = nullptr; E = nullptr;
}

void NVsequencer::update(int p, int a, int b)
{
	T[p] = E[a].abstick < E[b].abstick? a : b;
}
